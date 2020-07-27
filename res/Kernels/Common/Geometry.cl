#ifndef BRDFEDITOR_KERNEL_GEOMETRY_HEADER
#define BRDFEDITOR_KERNEL_GEOMETRY_HEADER

#include "Constants.cl"
#include "Types.cl"
 
//Solves quadratic equation in form a*x^2 + b*x + c = 0
//Returns: True if there's a solution and it's returned in *x0, if there are two 
bool solveQuad(float a, float b, float c, float* smaller,float* bigger)
{
	float D = b*b-4.0f*a*c;
	if(D<EPSILON) return false;
	else
    {
		float x1 = 0.5f* (-b - sqrt(D))/a; 
        float x2 = 0.5f* (-b + sqrt(D))/a;
        *smaller = min(x1,x2); 
        *bigger = max(x1,x2); 
    }
	return true;
}

//Generates orthonormal basis constructed from passed normal(of length 1).
//Return: two perpendicular vectors to the normal and each other
void genBasis(float3 n, float3* out1,float3* out2)
{
	
	*out1 = fabs(n.x)>fabs(n.z) ? (float3)(n.y,-n.x,0.0f) : (float3)(0.0f,n.z,-n.y);
	*out1 = normalize(*out1);
	*out2 = normalize(cross(n,*out1));

/*	*out1 = (float3)(n.z,0.0f,-n.x);
	float l = length(*out1);
	if(l < EPSILON)
		*out1 = (float3)(0.f,0.0f,1.0f);
	else
		*out1/=l;
	*out2 =normalize(cross(n,*out1));*/
}
 
//Generates ray from the camera based on threadID and offset in the pixel.
//--pixeloffset: [0,1]^2 where precisely should the ray intersect the pixel
Ray genCamRay(const Scene* scene,float2 pixelOffset)
{	
	float m = min(get_global_size(0),get_global_size(1));
	float2 offset = (float2)(get_global_id(0),get_global_id(1)) + pixelOffset;
	//-1.0,+1.0 coordinates on the screen
	//	- one size prolonged by aspect ration possibly.
	float2 rOffset = offset/m*2.0f - (float2)(get_global_size(0),get_global_size(1))/m;
	rOffset.y*=-1.0f;//y is up
	
	float3 right = cross(scene->camDir.xyz,scene->camUp.xyz);
	
	Ray r;
	r.o =scene->camPos.xyz;
	r.dir = scene->camDir.xyz + rOffset.x*right + rOffset.y*scene->camUp.xyz;
	r.dir = normalize(r.dir);
	r.tMax = FLT_MAX;
	return r;
}

#endif
