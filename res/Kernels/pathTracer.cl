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
			if(envMapIntensity>0.0f)
				LI += envMapIntensity*throughput*sampleHDRMap(envMap,ray.dir);
			break;
		}
		//If light
		else if(IS_LI(hitInfo.LI)){//Transfer photons
			LI+=throughput*hitInfo.LI;
			break;
		} 
		else{//Else object
			float3 hitPoint = hitInfo.hitPoint; 
			float3 wOut = -ray.dir;
			float3 n = hitInfo.worldNormal;
 
			//Bounce
			float pdf=0.0f;
			float3 wIn = genCosineDir(wOut,n,&rng,&pdf);
			//Eval BRDF, add to throughput
			throughput *=2* PI*BRDFEval(hitInfo.matInfo,n,wOut,wIn,matParams);
     
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