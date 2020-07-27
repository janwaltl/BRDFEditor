#ifndef BRDFEDITOR_KERNEL_TYPES_HEADER
#define BRDFEDITOR_KERNEL_TYPES_HEADER


//Holds the scene's parameters 
//Represents CL version of Renderer::Scene_cl c++ struct. Packed should hopefully ensure the correct size.
typedef struct __attribute__ ((packed)) Scene_tag
{
	//float4 because c++ code does not support float3. Fourth component is unused for cam***.
	float4 camPos;
	float4 camDir;//normalized
	float4 camUp;//normalized
	
	int numPointLights;
	int numAreaLights;
	int numSphereLights;

	int numSpheres;
	int numPlanes;
	int numCuboids;
	int numMeshes;
} Scene;

//Represents a point light
//Represents CL version of PointLight::PointLight_cl c++ struct. Packed should hopefully ensure the correct size.
typedef struct __attribute__ ((packed)) PointLight_tag
{
	//float4 because c++ code does not support float3. Fourth component is unused for both pos & color.
	float4 pos;
	float4 color;
	float intensity;
} PointLight;

//Represents an area light.
typedef struct __attribute__ ((packed)) AreaLight_tag
{
	// center.xyz = center
	// center.w = x,y,z half size ("radius")
	float4 centerSize;
	//.w unused
	float4 normal;
	//.xyz = RGB
	//.w intensity
	float4 colorIntensity;
} AreaLight;

typedef struct __attribute__ ((aligned (32))) SphereLight_tag
{
	//.xyz=center
	//.w=radius
	float4 centerRadius;
	//.xyz=RGB color
	//.w=Intensity
	float4 colorIntensity;
} SphereLight;

//centerRadius
typedef float4 Sphere;

typedef struct __attribute__ ((aligned (32))) Plane_t
{
	//c.w unused
	float4 center;
	//c.w unused
	float4 n;
} Plane;
typedef struct __attribute__ ((aligned (64))) Cuboid_t
{
	//Corner point + extents
	//p.w is unused
	//n_.xyz = normal
	//n_.w = length
	float4 p, nX,nY,nZ;
} Cuboid;

typedef struct __attribute__ ((aligned (64))) BVHNode_t
{
	int children[2];
	float childBounds[12];
	char padding[8];
} BVHNode;

typedef struct __attribute__ ((aligned (64))) Triangle_t
{
	float4 v0,e1,e2,n;
} Triangle;

typedef struct __attribute__ ((packed)) Mesh_t
{
	int begin,end,rootIndexNode;
} Mesh;

//A material parameter
typedef struct __attribute__ ((packed)) Param_t 
{
	union __attribute__ ((packed))
	{
		int i;
		float f1;
		float2 f2;
		float3 f3;
		float4 f4;
	}value;
}Param;

typedef struct __attribute__ ((packed)) MatInfo_t
{
	int brdfID;
	int paramOff;
} MatInfo;

typedef struct HitInfo_t
{
	//In world coordinates
	float3 hitPoint;
	float3 worldNormal;
	//What object did we hit
	//Use NOT_LI and IS_LI macros from Constants to set and test.
	float3 LI;
	//Otherwise this must be set
	MatInfo matInfo;
} HitInfo;

typedef struct Ray_t
{
	float3 o,dir;
	float tMax;
} Ray;

#define SCENE_OBJECTS_KERNEL_LIST 	const global Sphere* spheres,				\
									const global Plane* planes,				\
									const global Cuboid* cuboids,				\
									const global BVHNode* bvhNodes,			\
									const global Triangle* triangles,			\
									const global Mesh* meshes,				\
									const global PointLight* pLights,			\
									const global AreaLight* aLights,			\
									const global SphereLight* sLights,		\
									const Scene scene
#define SCENE_OBJECTS_LIST 	const global Sphere* spheres,				\
					 		const global Plane* planes,				\
							const global Cuboid* cuboids,				\
							const global BVHNode* bvhNodes,			\
							const global Triangle* triangles,			\
							const global Mesh* meshes,				\
							const global PointLight* pLights,			\
							const global AreaLight* aLights,			\
							const global SphereLight* sLights,		\
					 		const Scene* scene

#define PASS_SCENE_KERNEL_OBJECTS spheres,planes,cuboids,bvhNodes,triangles,meshes,pLights,aLights,sLights,&scene	
#define PASS_SCENE_OBJECTS spheres,planes,cuboids,bvhNodes,triangles,meshes,pLights,aLights,sLights,scene	
#endif