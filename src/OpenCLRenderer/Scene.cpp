#include "Scene.hpp"

#include <algorithm>

namespace brdfEditor::clRenderer {
Camera &
Scene::getCam() {
	return cam;
}
const Camera &
Scene::getCam() const {
	return cam;
}

Scene::Scene() : envMap(EnvMap::makeBlack(10, 10)) {
	recacheBRDFs();
	recacheMaterials();
}
std::vector<AreaLight> &
Scene::getAreaLights() {
	return areaLights;
}
const std::vector<AreaLight> &
Scene::getAreaLights() const {
	return areaLights;
}
std::vector<SphereLight> &
Scene::getSphereLights() {
	return sphereLights;
}
const std::vector<SphereLight> &
Scene::getSphereLights() const {
	return sphereLights;
}
std::vector<const Object *>
Scene::getObjects() const {
	std::vector<const Object *> objects;

	for (auto &&sphere : spheres)
		objects.push_back(&sphere);
	for (auto &&plane : planes)
		objects.push_back(&plane);
	for (auto &&cuboid : cuboids)
		objects.push_back(&cuboid);
	for (auto &&mesh : meshes)
		objects.push_back(&mesh);
	return objects;
}
std::vector<Object *>
Scene::getObjects() {
	std::vector<Object *> objects;

	for (auto &&sphere : spheres)
		objects.push_back(&sphere);
	for (auto &&plane : planes)
		objects.push_back(&plane);
	for (auto &&cuboid : cuboids)
		objects.push_back(&cuboid);
	for (auto &&mesh : meshes)
		objects.push_back(&mesh);
	return objects;
}
std::vector<TriangleMesh> &
Scene::getMeshes() {
	return meshes;
}
const std::vector<TriangleMesh> &
Scene::getMeshes() const {
	return meshes;
}
std::vector<PointLight> &
Scene::getPointLights() {
	return pointLights;
}
const std::vector<PointLight> &
Scene::getPointLights() const {
	return pointLights;
}

std::vector<Sphere> &
Scene::getSpheres() {
	return spheres;
}
const std::vector<Sphere> &
Scene::getSpheres() const {
	return spheres;
}
std::vector<Plane> &
Scene::getPlanes() {
	return planes;
}
const std::vector<Plane> &
Scene::getPlanes() const {
	return planes;
}
std::vector<Cuboid> &
Scene::getCuboids() {
	return cuboids;
}
const std::vector<Cuboid> &
Scene::getCuboids() const {
	return cuboids;
}
bool
Scene::createMaterial(std::string name, BRDFsCHandle brdfHandle) {
	bool inserted = materialsLibrary.createMaterial(name, brdfHandle);
	recacheMaterials();
	return inserted;
}
bool
Scene::createMaterial(std::string name, BRDFsHandle brdfHandle) {
	bool inserted = materialsLibrary.createMaterial(name, brdfHandle);
	recacheMaterials();
	return inserted;
}
bool
Scene::createMaterial(std::string name, BRDF brdf) {
	bool inserted =
		materialsLibrary.createMaterial(std::move(name), std::move(brdf));
	recacheMaterials();
	recacheBRDFs();
	return inserted;
}
bool
Scene::createMaterial(std::string name, std::string kernelSource) {
	bool inserted = materialsLibrary.createMaterial(std::move(name),
													std::move(kernelSource));
	recacheMaterials();
	recacheBRDFs();
	return inserted;
}
bool
Scene::createBRDF(std::string name, std::string kernelSource) {
	bool inserted =
		materialsLibrary.createBRDF(std::move(name), std::move(kernelSource));
	recacheBRDFs();
	return inserted;
}
bool
Scene::isMaterialUsed(const std::string &name) const {
	auto &&objects = getObjects();
	return std::any_of(objects.begin(), objects.end(), [&name](auto *object) {
		return object->getMatHandle()->first == name;
	});
}
bool
Scene::removeMaterial(const std::string &name) {

	if (isMaterialUsed(name))
		throw InUseSceneException(
			"Material '" + name +
			"'is being used by objects in the scene and so was not removed.");
	else {
		bool removed = materialsLibrary.removeMaterial(name);
		recacheMaterials();
		return removed;
	}
}
bool
Scene::isBRDFUsed(const std::string &name) const {
	auto &&maybeBRDF = getBRDF(name);
	if (!maybeBRDF)
		return false;
	auto &&[brdfName, brdf] = **maybeBRDF;

	return std::any_of(
		brdf.materials.begin(), brdf.materials.end(),
		[this](auto &&matName) { return this->isMaterialUsed(matName); });
}
bool
Scene::removeBRDF(const std::string &name) {
	if (isBRDFUsed(name))
		throw InUseSceneException("A material associated with BRDF '" + name +
								  "' is used by some object in the Scene. "
								  "BRDF was not removed but "
								  "some of its materials might have been.");

	auto &&maybeBRDFHandle = materialsLibrary.getBRDF(name);
	if (!maybeBRDFHandle)
		return false;
	auto &&[brdfName, brdf] = **maybeBRDFHandle;
	// Must copy, otherwise material will remove itself from it while iterating
	// over it.
	auto mats = brdf.materials;
	for (auto &&usedMat : mats)
		removeMaterial(usedMat);
	bool removed = materialsLibrary.removeBRDF(name);
	recacheBRDFs();
	return removed;
}
namespace {
template <typename Handle, typename Key, typename Value>
void
saveMapValues(std::vector<Handle> &cache, std::map<Key, Value> &map) {
	cache.clear();
	for (auto IT = map.begin(); IT != map.end(); ++IT)
		cache.push_back(IT);
}
} // namespace
const std::vector<MaterialCHandle> &
Scene::getMaterials() const {
	return cachedMatLibView.constMaterials;
}
const std::vector<MaterialHandle> &
Scene::getMaterials() {
	return cachedMatLibView.materials;
}
const std::vector<BRDFsCHandle> &
Scene::getBRDFs() const {
	return cachedMatLibView.constBRDFs;
}
const std::vector<BRDFsHandle> &
Scene::getBRDFs() {
	return cachedMatLibView.BRDFs;
}
std::optional<BRDFsCHandle>
Scene::getBRDF(const std::string &name) const {
	return materialsLibrary.getBRDF(name);
}
std::optional<BRDFsHandle>
Scene::getBRDF(const std::string &name) {
	return materialsLibrary.getBRDF(name);
}
std::optional<MaterialCHandle>
Scene::getMaterial(const std::string &name) const {
	return materialsLibrary.getMaterial(name);
}
std::optional<MaterialHandle>
Scene::getMaterial(const std::string &name) {
	return materialsLibrary.getMaterial(name);
}
void
Scene::setHDREnvironmentMap(const std::string &hdrMapFilename) {
	envMap = EnvMap(hdrMapFilename, 1.0f);
}
const EnvMap &
Scene::getEnvMap() const {
	return envMap;
}
EnvMap &
Scene::getEnvMap() {
	return envMap;
}
void
Scene::recacheMaterials() {
	materials_t &mats = materialsLibrary.getMaterials();
	saveMapValues(cachedMatLibView.materials, mats);
	saveMapValues(cachedMatLibView.constMaterials, mats);
}
void
Scene::recacheBRDFs() {
	BRDFS_t &brdfs = materialsLibrary.getBRDFs();
	saveMapValues(cachedMatLibView.BRDFs, brdfs);
	saveMapValues(cachedMatLibView.constBRDFs, brdfs);
}
} // namespace brdfEditor::clRenderer
