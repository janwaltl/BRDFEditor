
#define ENABLE_SHADOWS_FROM_LIGHTS
#define ENABLE_SHADOWS
#define ENABLE_LIGHT_INTERSECTIONS
#define ENABLE_MESHES
          
#include "Common/Constants.cl"
#include "Common/Geometry.cl"
#include "Common/IntersectionTests.cl"
#include "Common/LightSampling.cl"
#include "Common/Random.cl"
#include "Common/Types.cl"

//Declarations will be inserted here.
//It allows them to use e.g. RNGState type.
//DECLHERE

//Direct lighting - MIS
float3 sampleLightsMIS(SCENE_OBJECTS_LIST,
					RNGState* rng,
					HitInfo* hitInfo,
					global Param* matParams,
                    read_only image1d_t envMarginalRowHist,
					read_only image2d_t envRowHists,
					read_only image2d_t envMap,
					float envMapIntensity,
					float3 wOut)
{
	float3 LI=(float3)(0.0f);
	float3 hitPoint = hitInfo->hitPoint;
	float3 n = hitInfo->worldNormal;
	//PointLigts
	for(int i=0;i<scene->numPointLights;++i)
	{
		float3 lightPos = pLights[i].pos.xyz;
		float3 lightColor = pLights[i].color.xyz;
		float intensity = pLights[i].intensity;

		float3 wIn= normalize(lightPos - hitPoint);	
		float cosTheta = dot(wIn,n);
		//Trace shadow ray
		if(cosTheta< EPSILON || raySceneShadowTest(PASS_SCENE_OBJECTS,hitPoint,lightPos))
			continue;

		float dist2 = dot(lightPos-hitPoint,lightPos-hitPoint);
		float3 pLI=lightColor*intensity / (dist2+0.001f);			
		LI+=pLI*BRDFEval(hitInfo->matInfo,n,wOut,wIn,matParams)*cosTheta;
	}	 
	//Area lights
	for(int i=0;i<scene->numAreaLights;++i)
	{
			AreaLight light = aLights[i];

		//Light Sampling
		float lightPDF;
		float3 lightPos = sampleAreaLightPos(light,rng,&lightPDF);

		float3 wIn= normalize(lightPos - hitPoint);
		float cosTheta = dot(wIn,n);

		if(cosTheta<=EPSILON || raySceneShadowTest(PASS_SCENE_OBJECTS, hitPoint, lightPos))
			continue;
		float3 lightNormal =  light.normal.xyz;
		float dist2 = dot(hitPoint-lightPos,hitPoint-lightPos);
		lightPDF*= dist2/dot(lightNormal,-wIn);
		float3 aLI=light.colorIntensity.xyz*light.colorIntensity.w;
		float brdfPDF = GetwInPDF(hitInfo->matInfo,n,wOut,wIn, matParams);
		float weight = lightPDF/(lightPDF+brdfPDF);
		LI+=weight*aLI*BRDFEval(hitInfo->matInfo,n,wOut,wIn,matParams)*cosTheta/lightPDF;
	
		//BRDF Sampling		
		float3 val = BRDFSampleEval(hitInfo->matInfo,n,wOut,&wIn,rng,&brdfPDF,matParams);
		cosTheta = dot(wIn,n);	        
		if(cosTheta<EPSILON || brdfPDF<EPSILON)
			continue;

		Ray ray; ray.o = hitPoint + 0.001f*wIn; ray.dir = wIn; ray.tMax=FLT_MAX;
		HitInfo lightHit;
		lightPDF=0.0f;
		aLI=(float3)0.0f;
		//We've hit this light and not anything between
		if(rayAreaLightTest(light, &ray, &lightHit) && !raySceneShadowTest(PASS_SCENE_OBJECTS, lightHit.hitPoint, hitPoint)){
			float halfSize = light.centerSize.w;
			float3 lightNormal =  light.normal.xyz;
			float dist2 = dot(hitPoint-lightPos,hitPoint-lightPos);
			lightPDF=1/(halfSize*halfSize*4)* dist2/dot(lightNormal,-wIn);
		 	aLI=light.colorIntensity.xyz*light.colorIntensity.w;
		}
		weight = brdfPDF/(lightPDF+brdfPDF);
		LI+=weight*aLI*val*cosTheta/brdfPDF;
	}
	//Sphere lights
	for(int i=0;i<scene->numSphereLights;++i)
	{
		SphereLight light = sLights[i];
		//Light Sampling
		float lightPDF;
		float3 wIn;
		float3 lightPos = sampleSphereLightPos(light,hitPoint,rng,&wIn,&lightPDF);
		float cosTheta = dot(wIn,n);
		if(cosTheta<=EPSILON || raySceneShadowTest(PASS_SCENE_OBJECTS, hitPoint, lightPos))
			continue;
		//To solid angle measure
		float3 lightNormal =  normalize(lightPos - light.centerRadius.xyz);
		float dist2 = dot(hitPoint-lightPos,hitPoint-lightPos);
		lightPDF*= dist2/dot(lightNormal,-wIn);
		float3 sLI=light.colorIntensity.xyz*light.colorIntensity.w;

		float brdfPDF = GetwInPDF(hitInfo->matInfo,n,wOut,wIn, matParams);
		float weight = lightPDF/(lightPDF + brdfPDF);
		LI+=weight*sLI*BRDFEval(hitInfo->matInfo,n,wOut,wIn,matParams)*cosTheta/lightPDF;

		//BRDF Sampling		
		float3 val = BRDFSampleEval(hitInfo->matInfo,n,wOut,&wIn,rng,&brdfPDF,matParams);
		cosTheta = dot(wIn,n);	        
		if(cosTheta<EPSILON || brdfPDF<EPSILON)
			continue;

		Ray ray; ray.o = hitPoint + 0.001f*wIn; ray.dir = wIn; ray.tMax=FLT_MAX;
		HitInfo newHit;
		lightPDF=0.0f;
		sLI=(float3)0.0f;
		//We've hit this light and not anything between
		if(raySphereLightTest(light, &ray, &newHit) && !raySceneShadowTest(PASS_SCENE_OBJECTS, newHit.hitPoint, hitPoint)){
			float3 lightNormal =  normalize(newHit.hitPoint - light.centerRadius.xyz);
			float dist2 = dot(hitPoint-newHit.hitPoint,hitPoint-newHit.hitPoint);
			//Get lightPDF with respect to solid angle
			//Area:(cosine weighted not-unit hemisphere)
			//lightPDF=dot(lightNormal,-wIn)*1.0/(2*PI*light.centerRadius.w*light.centerRadius.w);
			//Solid angle:
			// - times * dist2/dot(lightNormal,-wIn)
			lightPDF= dist2/(2*PI*light.centerRadius.w*light.centerRadius.w);
		 	sLI=light.colorIntensity.xyz*light.colorIntensity.w;
		}
		weight = brdfPDF/(lightPDF+brdfPDF);
		LI+=weight*sLI*val*cosTheta/brdfPDF;
	}
	//HDR Map
	{
		//Light Sample
	    float3 wIn;
	    float3 sampledLI;
		float lightPDF;
	    dirSampleHDRMap(envMarginalRowHist, envRowHists,envMap,rng,&wIn,&sampledLI,&lightPDF);
		float brdfPDF = GetwInPDF(hitInfo->matInfo,n,wIn,wOut, matParams);
		float weight = lightPDF/(lightPDF+brdfPDF);
	    float cosTheta = dot(normalize(wIn),normalize(n));
	    if(dot(sampledLI,sampledLI)>EPSILON && cosTheta>EPSILON && !raySceneShadowTest(PASS_SCENE_OBJECTS, hitPoint+10000.0f*wIn,hitPoint))
			LI+=0.0f*weight*envMapIntensity * sampledLI/lightPDF*cosTheta*BRDFEval(hitInfo->matInfo,n,wOut,wIn,matParams);

		//BRDF Sample
		float3 val = BRDFSampleEval(hitInfo->matInfo,n,wOut,&wIn,rng,&brdfPDF,matParams);
		cosTheta = dot(wIn,n);	        
		if(cosTheta<EPSILON || brdfPDF<EPSILON)
			return LI;

		lightPDF=0.0f;
		sampledLI=(float3)0.0f;
		//We've did not hit anything = hit the HDR map
		if(!raySceneShadowTest(PASS_SCENE_OBJECTS, hitPoint, hitPoint+100*wIn)){		
		 	sampledLI=sampleHDRMapPDF(envMarginalRowHist,envRowHists,envMap,wIn,&lightPDF);
		}
		if(dot(sampledLI,sampledLI)<EPSILON || lightPDF < EPSILON)
			return LI;
		weight = brdfPDF/(lightPDF+brdfPDF);
		LI+=weight*sampledLI*val*cosTheta/brdfPDF;
	}
	return LI;
}
      
__kernel void render(SCENE_OBJECTS_KERNEL_LIST,
					 MATERIALS_AND_PARAMETERS,
					 IMAGE,
					 RNG,
			 		 ENV_MAP)
{  
	int tID= get_global_id(0) + get_global_size(0)*get_global_id(1);
	RNGState rng = RNG[tID];     
	Ray ray = genCamRay([...]);
	HitInfo hitInfo;
	bool hitAnything =false;  
	float3 LI=(float3)(0.0f);   
	float3 throughput=(float3)(1.0f);
                  
	for(int bounce=0;bounce<MAX_BOUNCES;++bounce){ 
		//Hit scene 
		hitAnything,ray,hitInfo = raySceneTest([...]);
		//If nothing
		if(!hitAnything){//HDR Map
			if(bounce==0)
			 	LI += [MAP's LIGHT]
			break;
		} 
		//Hit a light
		else if(IS_LI(hitInfo.LI)){//Transfer photons
			LI+=bounce==0 ? [LIGHT];
			break;
		}             
		else{//Hit an object	
			//Direct Lighting 
			LI+=throughput*sampleLightsMIS([...]);
			//Bounce,Evaluate the BRDF, add to throughput 
			float3 wIn;  float pdf;
			float3 val,wIn,pdf = BRDFSampleEval(...);
			float cosTheta = dot(wIn,n);	
			//Is a bounce
			if(cosTheta<EPSILON || pdf<EPSILON)   
				break;	 	
			throughput*= val*cosTheta/pdf;
                    
			[RUSSIAN ROULETTE]
			[UPDATE RAY]
		}
	} 
	[WRITE TO IMAGE]
}   