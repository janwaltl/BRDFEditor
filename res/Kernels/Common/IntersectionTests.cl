#ifndef BRDFEDITOR_KERNEL_INTERSECTION_TESTS_HEADER
#define BRDFEDITOR_KERNEL_INTERSECTION_TESTS_HEADER

#include "Constants.cl"
#include "Types.cl"
#include "Geometry.cl"

typedef struct RaySphereHitInfo_t
{
    float3 sR;
    float t;
    bool hit;
}SphereHitInfo;

typedef struct RayPlaneHitInfo_t
{
    bool hit;
    float3 planeN;
    float t;
}RayPlaneHitInfo;

typedef struct RayTriangleHitInfo_t
{
    bool hit;
    float3 triN;
    float t;
}RayTriangleHitInfo;

typedef struct RayCuboidHitInfo_t
{
	float3 normal;
	float t;
	bool hit;
}RayCuboidHitInfo;

// Tests passed ray against the sphere. The ray must not start inside the sphere.
// Returns: Whether the ray hit the sphere and in that case updates HitInfo and sets r->tMax to that hit-point.
bool raySphereTest(float4 sphere, const Ray* r,SphereHitInfo* hitInfo)
{
	if(r->tMax<=0.0f)
		return hitInfo->hit=false;
	//solves ||x-sphere.xyz||^2 = r^2 ; x=r->o + t*r->dir;  for t
	
	float3 sR = r->o - sphere.xyz;//Vector from sphere to ray's origin
	
	//t*t*dot(r->dir,r->dir) + 2*t*dot(r->dir,sR) + dot(sR,sR)-r^2=0
	float a = 1.0f;//r->dir is normalized
	float b = 2.0f*dot(r->dir,sR);
	float c = dot(sR,sR) - sphere.w*sphere.w;
	float t,tBigger;
	
	//No hit or too far
	if(!solveQuad(a,b,c,&t,&tBigger) || t>r->tMax || t<=0.0f)
		return hitInfo->hit=false;	

	hitInfo->t=t;
    hitInfo->sR=sR;
	return hitInfo->hit=true;
}

void processSphereHitInfo(const SphereHitInfo* sInfo,MatInfo matInfo, HitInfo* hit, Ray* ray)
{
    if(!sInfo->hit)
        return;
    ray->tMax=sInfo->t;
    hit->hitPoint = ray->o+sInfo->t*ray->dir;
    //r->o + t*r->dir - sphere.xyz = vector from the sphere's center to the hitPoint.
    hit->worldNormal = normalize(sInfo->sR + sInfo->t*ray->dir);
    hit->matInfo = matInfo;
	hit->LI = NOT_LI;
}

void clipSides(float* tmin, float* tmax,float3* normal, const Ray* r,float3 o, float ext, float3 n){
	float d,t1,t2;
	d = dot(r->dir, n);
	if(fabs(d) > EPSILON){
		t1 = dot((o - r->o),n)/d;
		t2 = dot((o + ext*n - r->o),n)/d;
		float newMin = fmin(t1, t2);
		if(newMin>*tmin){
			*tmin=newMin;
			*normal = sign(t1-t2)*n;
		}
        *tmax = fmin(*tmax, fmax(t1, t2));
	}
}
bool rayCuboidTest(Cuboid c, const Ray* r, RayCuboidHitInfo* hitInfo){
	float tmin = -FLT_MAX, tmax = FLT_MAX;
	float3 o = c.p.xyz;
	float3 nX = c.nX.xyz;
	float3 nY = c.nY.xyz;
	float3 nZ = c.nZ.xyz;
	float extX = c.nX.w;
	float extY = c.nY.w;
	float extZ = c.nZ.w;
	float3 normal;
	clipSides(&tmin,&tmax,&normal,r,o,extX,nX);
	clipSides(&tmin,&tmax,&normal,r,o,extY,nY);
	clipSides(&tmin,&tmax,&normal,r,o,extZ,nZ);

	hitInfo->t = tmin;
	hitInfo->normal = normal;
	return hitInfo->hit = tmin<=tmax && tmin<r->tMax && tmin>=0.0f;
}
void processCuboidHitInfo(const RayCuboidHitInfo* cInfo, MatInfo matInfo, HitInfo* hit, Ray* ray){
	if(!cInfo->hit)
		return;
	ray->tMax = cInfo->t;
	hit->hitPoint = ray->o+cInfo->t*ray->dir;
	hit->worldNormal = cInfo->normal;
	hit->matInfo = matInfo;
	hit->LI = NOT_LI;
}
bool rayPlaneTest(Plane plane, const Ray* r,RayPlaneHitInfo* hitInfo)
{
	float3 planeN = plane.n.xyz;
	float3 plane0 = plane.center.xyz;
	float d = dot(-r->dir,planeN);
	if(d>=EPSILON)
	{
		float t = dot((r->o - plane0),planeN)/d;
		if(t>=0 && t<=r->tMax)
		{
			hitInfo->planeN=planeN;
            hitInfo->t=t;	
			return hitInfo->hit=true;
		}
	}
	return hitInfo->hit=false;
}
void processPlaneHitInfo(const RayPlaneHitInfo* pInfo,MatInfo matInfo, HitInfo* hit, Ray* ray)
{
    if(!pInfo->hit)
        return;
    ray->tMax=pInfo->t;
	hit->worldNormal=pInfo->planeN;
    hit->hitPoint = ray->o+pInfo->t*ray->dir;
    hit->matInfo=matInfo;
	hit->LI=NOT_LI;
}


bool rayTriangleTest(Triangle tri, const Ray* ray, RayTriangleHitInfo* hitInfo)
{
	float3 v0=tri.v0.xyz;
	float3 e1=tri.e1.xyz;
	float3 e2=tri.e2.xyz;

	hitInfo->hit=false;
	if(fabs(dot(tri.n.xyz,ray->dir))<EPSILON)
		return false;
	float3 pvec = cross(ray->dir,e2);
	float det = dot(e1,pvec);
	if(det<EPSILON && det>-EPSILON)
		return false;
	float f= 1.0f/det;
	float3 dist=ray->o-v0;
	float u =dot(pvec,dist)*f;
	if(	u<0.0f || u>1.0f)
		return false;
	float3 qvec = cross(dist,e1);
	float v = dot(ray->dir,qvec) * f;
	if(v<0.0f || u+v>1.0f)
		return false;
	float t = f* dot(e2,qvec);
	if(t>0.0f && t < ray->tMax)
	{
		hitInfo->t=t;
		hitInfo->triN=tri.n.xyz;
		return hitInfo->hit=true;
	}
	return false;
}
void processTriangleHitInfo(const RayTriangleHitInfo* tInfo,MatInfo matInfo, HitInfo* hit, Ray* ray)
{
    if(!tInfo->hit)
        return;
    ray->tMax=tInfo->t;
	hit->worldNormal=tInfo->triN;
	hit->hitPoint=ray->o+tInfo->t*ray->dir;
   	hit->matInfo=matInfo;
	hit->LI = NOT_LI;
}
bool rayAABBTest(float3 AABBmin,float3 AABBmax,Ray* r)
{
	//https://tavianator.com/fast-branchless-raybounding-box-intersections/
	float tmin = -FLT_MAX, tmax = FLT_MAX;
	float t1,t2;

	if (r->dir.x > EPSILON || r->dir.x < - EPSILON) {
        t1 = (AABBmin.x - r->o.x)/r->dir.x;
        t2 = (AABBmax.x - r->o.x)/r->dir.x;
 
        tmin = fmax(tmin, fmin(t1, t2));
        tmax = fmin(tmax, fmax(t1, t2));
    }

	if (r->dir.y > EPSILON || r->dir.y < - EPSILON) {
        t1 = (AABBmin.y - r->o.y)/r->dir.y;
        t2 = (AABBmax.y - r->o.y)/r->dir.y;
 
        tmin = fmax(tmin, fmin(t1, t2));
        tmax = fmin(tmax, fmax(t1, t2));
    }

	if (r->dir.z > EPSILON || r->dir.z < - EPSILON) {
        t1 = (AABBmin.z - r->o.z)/r->dir.z;
        t2 = (AABBmax.z - r->o.z)/r->dir.z;
 
        tmin = fmax(tmin, fmin(t1, t2));
        tmax = fmin(tmax, fmax(t1, t2));
    }
	
	return tmin<=tmax && tmin<r->tMax && tmin>=0.0f;
}

bool rayBVHTest(int rootNode,const global BVHNode* nodes, const global Triangle* triangles, MatInfo* matInfo,Ray* r,HitInfo* hit,bool shadow)
{
	bool hitTest=false;
	#define STACK_SIZE 32
	int stack[STACK_SIZE];
	int stackTop=0;
	stack[0]=rootNode;
	RayTriangleHitInfo tInfo;
	while(stackTop>=0)
	{
		BVHNode node=nodes[stack[stackTop--]];
		
		if(node.children[0]>=0)//Internal node
		{
			if(rayAABBTest(vload3(0,&node.childBounds[0]),vload3(1,&node.childBounds[0]),r))
				stack[++stackTop]=node.children[0];
			if(rayAABBTest(vload3(2,&node.childBounds[0]),vload3(3,&node.childBounds[0]),r))
				stack[++stackTop]=node.children[1];
		}
		else
		{
			for(int i=0;i<-node.children[0];++i)
			{
				int tIndex = node.children[1]+i;
				hitTest |=rayTriangleTest(triangles[tIndex],r,&tInfo);
				if(shadow && hitTest)
					return true;
				processTriangleHitInfo(&tInfo,*matInfo,hit,r);
			}
		}
	}
	
	return hitTest;
}
bool rayAreaLightTest(AreaLight aLight, Ray* r, HitInfo* hit){
	RayPlaneHitInfo pInfo;
	Plane plane;
	plane.center.xyz = aLight.centerSize.xyz;
	plane.n.xyz = aLight.normal.xyz;
	bool planeHit = rayPlaneTest(plane,r, &pInfo);
	if(!planeHit)
        return false;
	//Test for size
	float3 hitPoint = r->o+pInfo.t*r->dir;
	float3 diffVec = fabs(aLight.centerSize.xyz - hitPoint);
	if(max(diffVec.x,max(diffVec.y,diffVec.z))>aLight.centerSize.w)
		return false;

    r->tMax=pInfo.t;
	hit->worldNormal=pInfo.planeN;
    hit->hitPoint = hitPoint;
	hit->LI=aLight.colorIntensity.xyz * aLight.colorIntensity.w;
	return true;
}
bool raySphereLightTest(SphereLight sLight, Ray* r, HitInfo* hit){
	SphereHitInfo sInfo; 
    bool sphereHit= raySphereTest(sLight.centerRadius,r,&sInfo);
	if(!sphereHit)
		return false;
	r->tMax=sInfo.t;
    hit->hitPoint = r->o+sInfo.t*r->dir;
    //r->o + t*r->dir - sphere.xyz = vector from the sphere's center to the hitPoint.
    hit->worldNormal = normalize(sInfo.sR + sInfo.t*r->dir);
	hit->LI = sLight.colorIntensity.xyz * sLight.colorIntensity.w;
	return true;
}
//Traces the ray against the scene.
//Returns whether the ray hit anything and in that case updates HitInfo and sets r->tMax to that hit-point.
bool raySceneTest(SCENE_OBJECTS_LIST,
				  global MatInfo* matInfos,
			  	  Ray* ray,
			  	  HitInfo* hit)
{
	bool hitTest=false;

	//Objects
	uint matOffset=0;
	for(int i=0;i<scene->numSpheres;++i){
    	SphereHitInfo sInfo;
        bool res= raySphereTest(spheres[i],ray,&sInfo);
        processSphereHitInfo(&sInfo,matInfos[matOffset+i],hit,ray);
        hitTest|=res;
    }
	matOffset+=scene->numSpheres;

	for(int i=0;i<scene->numPlanes;++i){
    	RayPlaneHitInfo pInfo;
        bool res= rayPlaneTest(planes[i],ray,&pInfo);
        processPlaneHitInfo(&pInfo,matInfos[matOffset+i],hit,ray);
        hitTest|=res;
    }
	matOffset+=scene->numPlanes;

	for(int i=0;i<scene->numCuboids;++i){
    	RayCuboidHitInfo cInfo;
        bool res= rayCuboidTest(cuboids[i],ray,&cInfo);
        processCuboidHitInfo(&cInfo,matInfos[matOffset+i],hit,ray);
        hitTest|=res;
    }
	matOffset+=scene->numCuboids;
#ifdef ENABLE_MESHES
	for(int i=0;i<scene->numMeshes;++i)
	{
		Mesh mesh=meshes[i];
		MatInfo mat= matInfos[matOffset+i];
		hitTest|=rayBVHTest(mesh.rootIndexNode,bvhNodes,triangles,&mat,ray,hit,false);
	}
#endif
#ifdef ENABLE_LIGHT_INTERSECTIONS
	//Lights
	//No point lights
	for(int i=0;i<scene->numAreaLights;++i){
        bool res= rayAreaLightTest(aLights[i], ray, hit);
        hitTest|=res;
    }
	for(int i=0;i<scene->numSphereLights;++i){
        bool res= raySphereLightTest(sLights[i], ray, hit);
        hitTest|=res;
    }
#endif
	return hitTest;
}

bool raySceneShadowTest(SCENE_OBJECTS_LIST,
					float3 from,
					float3 to)
{
#ifdef ENABLE_SHADOWS
	Ray ray;
	ray.o = from;
	ray.dir = to-from;
	float dist = sqrt(dot(ray.dir,ray.dir));
	ray.dir/=dist;
	ray.o+=EPSILON*ray.dir;
	//TODO Precise bound
	ray.tMax=dist*0.99f;

	//Objects
	uint matOffset=0;
	for(int i=0;i<scene->numSpheres;++i){
    	SphereHitInfo sInfo;
        if(raySphereTest(spheres[i],&ray,&sInfo))
			return true;
    }
	matOffset+=scene->numSpheres;

	for(int i=0;i<scene->numPlanes;++i){
    	RayPlaneHitInfo pInfo;
        if(rayPlaneTest(planes[i],&ray,&pInfo))
			return true;
    }
	matOffset+=scene->numPlanes;

	for(int i=0;i<scene->numCuboids;++i){
    	RayCuboidHitInfo cInfo;
        if(rayCuboidTest(cuboids[i],&ray,&cInfo))
			return true;
    }
	matOffset+=scene->numPlanes;
	HitInfo dummy;
#ifdef ENABLE_MESHES
	for(int i=0;i<scene->numMeshes;++i)
	{
		Mesh mesh=meshes[i];
		MatInfo dummyMat;
		if(rayBVHTest(mesh.rootIndexNode,bvhNodes,triangles,&dummyMat,&ray,&dummy,true))
			return true;
	}
#endif
#ifdef ENABLE_SHADOWS_FROM_LIGHTS
	//Lights
	//No point lights
	for(int i=0;i<scene->numAreaLights;++i){
        if(rayAreaLightTest(aLights[i], &ray, &dummy))
			return true;
    }
	for(int i=0;i<scene->numSphereLights;++i){
        if(raySphereLightTest(sLights[i], &ray, &dummy))
			return true;
    }
#endif
#endif
	return false;
}


#endif