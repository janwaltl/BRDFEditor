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


//Direct lighting - light PDF
float3 sampleLights(SCENE_OBJECTS_LIST,
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
		float3 aLI=sampleAreaLight(light,lightPos,hitPoint);
		LI+=aLI*BRDFEval(hitInfo->matInfo,n,wOut,wIn,matParams)*cosTheta/lightPDF;
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
		float3 sLI=sampleSphereLight(light, wIn, hitPoint, lightPos);
		LI+=sLI*BRDFEval(hitInfo->matInfo,n,wOut,wIn,matParams)*cosTheta/lightPDF;
	}
	if(envMapIntensity>EPSILON)
	{
		//Light Sample
	    float3 wIn;
	    float3 sampledLI;
		float lightPDF;
	    dirSampleHDRMap(envMarginalRowHist, envRowHists,envMap,rng,&wIn,&sampledLI,&lightPDF);
	    float cosTheta = dot(normalize(wIn),normalize(n));
	    if(dot(sampledLI,sampledLI)>EPSILON && cosTheta>EPSILON && !raySceneShadowTest(PASS_SCENE_OBJECTS, hitPoint+10000.0f*wIn,hitPoint))
			LI+=envMapIntensity * sampledLI*BRDFEval(hitInfo->matInfo,n,wOut,wIn,matParams)*cosTheta/lightPDF;
	}
	return LI;
}

__kernel void render(SCENE_OBJECTS_KERNEL_LIST,
					 global MatInfo* matInfos,
					 global Param* matParams,
					 global float4 * buffImage,
					 global uint2* rngState,
			 		 read_only image1d_t envMarginalRowHist,
					 read_only image2d_t envRowHists,
					 read_only image2d_t envMap,
					 float envMapIntensity,
					 int numSamples,
					 int maxBounces)
{
	int tID= get_global_id(0) + get_global_size(0)*get_global_id(1);
	RNGState rng = rngState[tID];
 
	float2 pixelOffset = (float2)(uniformRNG(&rng), uniformRNG(&rng));
	Ray ray = genCamRay(&scene,pixelOffset);

	HitInfo hitInfo;
	bool hitAnything =false;  
	float3 LI=(float3)(0.0f);
	float3 throughput=(float3)(1.0f);
      
	for(int bounce=0;bounce<maxBounces;++bounce)
	{ 
		//Hit scene   
		hitAnything = raySceneTest(PASS_SCENE_KERNEL_OBJECTS,matInfos,&ray,&hitInfo);

		//If nothing
		if(!hitAnything){//HDR Map
			if(envMapIntensity>0.0f && bounce==0)
				LI += envMapIntensity*throughput*sampleHDRMap(envMap,ray.dir);
			break;
		}   
		//If light
		else if(IS_LI(hitInfo.LI)){//Transfer photons
			LI+=bounce==0 ? throughput*hitInfo.LI : 0.0f;
			break; 
		}     
		else{//Else object
			float3 hitPoint = hitInfo.hitPoint; 
			float3 wOut = -ray.dir;
			float3 n = hitInfo.worldNormal;
			//Direct Lighting
			LI+=throughput*sampleLights(PASS_SCENE_KERNEL_OBJECTS,&rng,&hitInfo,matParams,envMarginalRowHist,envRowHists,envMap,envMapIntensity,wOut);
			//Bounce
			float pdf;
			float3 wIn = genCosineDir(wOut,n,&rng,&pdf);
			float cosTheta = dot(n,wIn);
			if(cosTheta<=0.0f) break;
			//Eval BRDF, add to throughput
			throughput *= BRDFEval(hitInfo.matInfo,n,wOut,wIn,matParams);
        
			//Russian roulette
			float threshold = max(throughput.x, max(throughput.y,throughput.z));
			if(uniformRNG(&rng)<threshold)
				throughput/=threshold;
			else
		 		break;
			//Set new ray
			ray.dir=normalize(wIn);
			//Offset slightly so we don't hit the same object
			ray.o=hitPoint+wIn*0.001f;//RESOLVE Epsilon
			ray.tMax=FLT_MAX;
		}
	} 
	      
	rngState[tID] = rng;
	float4 newCol = (float4)(LI,1.0f);
	if(numSamples>1)//Add previous 
		newCol+=buffImage[tID];
	//Otherwise the buffer will be overwritten
	buffImage[tID]= newCol;	

}