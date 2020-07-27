#ifndef BRDFEDITOR_OPENCL_SCENE_HEADER
#define BRDFEDITOR_OPENCL_SCENE_HEADER

#include <string>
#include <vector>
#include <optional>

#include "Camera.hpp"
#include "EnvMap.hpp"
#include "Materials/MaterialsLibrary.hpp"
#include "Lights/AreaLight.hpp"
#include "Lights/PointLight.hpp"
#include "Lights/SphereLight.hpp"
#include "Objects/Sphere.hpp"
#include "Objects/Plane.hpp"
#include "Objects/Cuboid.hpp"
#include "Objects/TriangleMesh.hpp"

namespace brdfEditor {
namespace clRenderer {

// Exception class used by Scene to signal a to-be-removed BRDF or material is
// used by objects and so cannot be removed.
class InUseSceneException : public Exception {
public:
	using Exception::Exception;
};
// Represents a scene. Holds all scene objects and their materials&BRDFs.
class Scene {
public:
	// Creates an empty scene with some materials.
	Scene();

	// Creates a material from the already present source.
	// --brdfHandle: Must be obtained and present in this instance's materials.
	// returns: Whether the material was created. Returns false if a material
	// with given name already exists.
	bool
	createMaterial(std::string name, BRDFsCHandle brdfHandle);
	// See BRDFsCHandle overload.
	bool
	createMaterial(std::string name, BRDFsHandle brdfHandle);
	// Adds a new material and its source to the library.
	// returns: Whether the BRDF and material were created & inserted. Returns
	// false if either a BRDF or material with the same name exists. In that
	// case neither is inserted.
	bool
	createMaterial(std::string name, BRDF brdf);
	// Adds a new material and its source to the library.
	// Material source is also created with the same name.
	// returns: Whether the BRDF and material were created & inserted. Returns
	// false if either a BRDF or material with the same name exists. In that
	// case neither is inserted.
	bool
	createMaterial(std::string name, std::string kernelSource);
	// Adds a new BRDF and its source tot the library.
	// Returns: Whether the BRDF was created and inserted. Retuns false if a
	// BRDF with the same name exists.
	bool
	createBRDF(std::string name, std::string kernelSource);
	// Returns: Whether an object is using the material. Returns false even if
	// it does not exist.
	bool
	isMaterialUsed(const std::string &name) const;
	// Removes material from the scene. Material must not be used by any objects
	// in the scene.
	// Returns: Whether the material was removed. False if a
	// material with such name did not exists.
	// Throws: InUseSceneException if an object uses this material.
	bool
	removeMaterial(const std::string &name);
	// Returns: Whether an object is using a material associated with this BRDF.
	// Returns false even if the BRDF does not exist.
	bool
	isBRDFUsed(const std::string &name) const;
	// Tries to remove the BRDf with passed name form the scene. First removes
	// all materials associated with this BRDF using Scene::removeMaterial, see
	// its docs.
	// Returns: Whether the BRDF was removed. False if a BRDf with such name did
	// not exists. Throws: InUseSceneException if an object uses any
	// associtiated materials. In that case some materials might have been
	// already deleted and will not be added back - No strong exception
	// guarantee.
	bool
	removeBRDF(const std::string &name);

	Camera &
	getCam();

	const Camera &
	getCam() const;

	// Returns: All objects in the scene - sphere, planes, cuboids, meshes.
	// Does not return lights. Objects a returned in ^ order.
	std::vector<const Object *>
	getObjects() const;
	// See const overload.
	std::vector<Object *>
	getObjects();

	std::vector<AreaLight> &
	getAreaLights();

	const std::vector<AreaLight> &
	getAreaLights() const;

	std::vector<SphereLight> &
	getSphereLights();

	const std::vector<SphereLight> &
	getSphereLights() const;

	std::vector<PointLight> &
	getPointLights();

	const std::vector<PointLight> &
	getPointLights() const;

	std::vector<Sphere> &
	getSpheres();

	const std::vector<Sphere> &
	getSpheres() const;

	std::vector<Plane> &
	getPlanes();

	const std::vector<Plane> &
	getPlanes() const;

	std::vector<Cuboid> &
	getCuboids();

	const std::vector<Cuboid> &
	getCuboids() const;

	std::vector<TriangleMesh> &
	getMeshes();

	const std::vector<TriangleMesh> &
	getMeshes() const;

	const std::vector<MaterialCHandle> &
	getMaterials() const;

	const std::vector<MaterialHandle> &
	getMaterials();

	const std::vector<BRDFsCHandle> &
	getBRDFs() const;

	const std::vector<BRDFsHandle> &
	getBRDFs();
	// Returns handle to the BRDF with the given name if it exists. The handle
	// is valid until this library exists and the material has not been removed.
	std::optional<BRDFsCHandle>
	getBRDF(const std::string &name) const;
	// See const version.
	std::optional<BRDFsHandle>
	getBRDF(const std::string &name);
	// Returns handle to the material with the given name if it exists. The
	// handle is valid until this library exists and the material has not been
	// removed.
	std::optional<MaterialCHandle>
	getMaterial(const std::string &name) const;
	// See const version.
	std::optional<MaterialHandle>
	getMaterial(const std::string &name);

	void
	setHDREnvironmentMap(const std::string &hdrMapFilename);
	const EnvMap&
	getEnvMap() const;
	 EnvMap&
		getEnvMap();
private:
	// Updates 'cachedMatLibView' materials from 'materialsLibrary'.
	void
	recacheMaterials();
	// Updates 'cachedMatLibView' materials from 'materialsLibrary'.
	void
	recacheBRDFs();

	Camera cam;

	std::vector<Sphere> spheres;
	std::vector<Plane> planes;
	std::vector<Cuboid> cuboids;
	std::vector<TriangleMesh> meshes;

	std::vector<AreaLight> areaLights;
	std::vector<PointLight> pointLights;
	std::vector<SphereLight> sphereLights;
	EnvMap envMap;

	MaterialsLibrary materialsLibrary;
	struct {
		std::vector<MaterialCHandle> constMaterials;
		std::vector<MaterialHandle> materials;
		std::vector<BRDFsCHandle> constBRDFs;
		std::vector<BRDFsHandle> BRDFs;
	} cachedMatLibView;
};
} // namespace clRenderer
} // namespace brdfEditor
#endif // !BRDFEDITOR_OPENCL_SCENE_HEADER
