#ifndef BRDFEDITOR_KERNEL_LIGHT_SAMPLING_HEADER
#define BRDFEDITOR_KERNEL_LIGHT_SAMPLING_HEADER

#include "Constants.cl"
#include "IntersectionTests.cl"
#include "Geometry.cl"
#include "Random.cl"
#include "Types.cl"

float3 BRDFEval(MatInfo matInfo, float3 normal, float3 wIn, float3 wOut, __global Param* paramBuffer);
float3 BRDFSampleEval(MatInfo matInfo, float3 normal, float3 wIn, float3* wOut, RNGState* rngState, float* pdf, __global Param* paramBuffer);
float GetwInPDF(MatInfo matInfo, float3 n, float3 wOut,float3 wIn, __global Param* paramBuffer);


float3 sampleHDRMap(read_only image2d_t sky, float3 dir)
{
	int w = get_image_width(sky);
	int h = get_image_height(sky);
	//longtitude [-PI,PI] -> [0,1]
	float u = -atan2(dir.z,dir.x)/(TWOPI) + 0.5;
	//latitude [0,PI] -> [0,1]
	float v = 0.5-asin(dir.y)/PI;

	return read_imagef(sky,(int2)(u*w,v*h)).xyz;
}
float3 sampleHDRMapPDF(read_only image1d_t envMarginalRowHist,
					  read_only image2d_t envRowHists,
					  read_only image2d_t envMap,
					  float3 dir,float* pdf)
{
	int w = get_image_width(envMap);
	int h = get_image_height(envMap);
	//longtitude [-PI,PI] -> [0,1]
	float u = -atan2(dir.z,dir.x)/(TWOPI) + 0.5;
	//latitude [0,PI] -> [0,1]
	float v = 0.5-asin(dir.y)/PI;

	int chosenRow = v*h;
	int chosenCol = u*w;
	*pdf = read_imagef(envMarginalRowHist,chosenRow).z;
	*pdf *=read_imagef(envRowHists,(int2)(chosenCol,chosenRow)).z;
	*pdf *= w*h/(4*PI);// Correction between unit sphere surface area and [w,h]^2
	return read_imagef(envMap,(int2)(chosenCol,chosenRow)).xyz;
}
void dirSampleHDRMap(read_only image1d_t envMarginalRowHist,
					 read_only image2d_t envRowHists,
					 read_only image2d_t envMap,
					 RNGState* rng,
					 float3* outDir,
					 float3* sampledLI,
					 float* pdf)
{
	//envMarginalRowHist.x == K table
	//envMarginalRowHist.y == V table
	//envMarginalRowHist.z == PDF

	int w = get_image_width(envMap);
	int h = get_image_height(envMap);

	//Marginal
	float u = uniformRNG(rng);
	float v = uniformRNG(rng);
	int j = u*h;

	float4 j_hist = read_imagef(envMarginalRowHist,j);
	int chosenRow;
	float chosenRowPdf;
	if(v < j_hist.y){
		chosenRow = j_hist.x;
		chosenRowPdf = read_imagef(envMarginalRowHist,chosenRow).z;
	}else{
		chosenRow = j;
		chosenRowPdf = j_hist.z;
	}
	//Row
	u = uniformRNG(rng);
	v = uniformRNG(rng);
	j = u*w;

	j_hist = read_imagef(envRowHists,(int2)(j, chosenRow));
	int chosenCol;
	float chosenColPdf;
	if(v < j_hist.y){
		chosenCol = j_hist.x;
		chosenColPdf = read_imagef(envRowHists,(int2)(chosenCol,chosenRow)).z;
	}else{
		chosenCol = j;
		chosenColPdf = j_hist.z;
	}

	u = chosenRow/(float)h;
	v = chosenCol/(float)w;

	//u = uniformRNG(rng);
	//v = uniformRNG(rng);

	//Mappings are made to match `sampleHDRMap` and `sampleHDRMapPDF` functions
	float3 hor = normalize((float3)(cos((1.0f-v)*TWOPI + PI),0.0f,sin((1.0f-v)*TWOPI + PI)));
	*outDir = hor*cos(-u*PI + HALFPI) + (float3)(0.0f,sin(-u*PI + HALFPI),0.0f);

	//Map sampling from an area of w*h to 4*PI
	// - factor: w*h/(4*PI) 
	*pdf = chosenColPdf * chosenRowPdf*w*h/(4*PI);
	*sampledLI = read_imagef(envMap,(int2)(v*w,u*h)).xyz;
	//*pdf = 1/(4*PI);
}

//Sample a position on the light
float3 sampleAreaLightPos(AreaLight light, RNGState* rng, float* pdf)
{
	float halfSize = light.centerSize.w;
	float3 out1,out2;
	genBasis(light.normal.xyz,&out1,&out2);
	float e1 = halfSize*2.0f*(uniformRNG(rng)-0.5f);
	float e2 = halfSize*2.0f*(uniformRNG(rng)-0.5f);
	*pdf = 1/(halfSize*halfSize*4);
	return light.centerSize.xyz + out1*e1 + out2*e2;
}

//Returns LI from given light assuming that it is unobstructed
float3 sampleAreaLight(AreaLight light, float3 sampledLPos, float3 hitPos)
{	
	float3 sampledDir=normalize(hitPos-sampledLPos);
	float dist2 = dot(sampledLPos-hitPos,sampledLPos-hitPos);
	//Light are bidirectional
	return light.colorIntensity.w*light.colorIntensity.xyz*fabs(dot(sampledDir,light.normal.xyz))/(dist2 + EPSILON);
}

//Sample a position on the light
float3 sampleSphereLightPos(SphereLight light, float3 hitPoint, RNGState* rng,float3 *wIn, float* pdf)
{	
	float3 lightNormal =  normalize(hitPoint - light.centerRadius.xyz);
	float3 dir = genCosineDirN(lightNormal, rng, pdf);
	float3 lightPoint = light.centerRadius.xyz + light.centerRadius.w * dir;
	//Go from unit hemisphere area to radius hemisphere area
	*pdf*=1.0f/(2*PI*light.centerRadius.w*light.centerRadius.w);
	*wIn = normalize(lightPoint - hitPoint);
	return lightPoint;
}

float3 sampleSphereLight(SphereLight light,float3 wIn, float3 hitPoint, float3 lightPoint){

	float3 lightNormal = (lightPoint-light.centerRadius.xyz)/light.centerRadius.w;
	float dist2 = dot(lightPoint-hitPoint,lightPoint-hitPoint);

	return light.colorIntensity.xyz*light.colorIntensity.w*dot(lightNormal,-wIn)/ (dist2);
}

#endif