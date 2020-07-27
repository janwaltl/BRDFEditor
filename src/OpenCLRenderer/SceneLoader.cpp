#include "SceneLoader.hpp"

#include <algorithm>
#include <string>
#include <fstream>
#include <streambuf>

#include <glm/common.hpp>
#include <json11/json11.hpp>

namespace brdfEditor::clRenderer {

namespace {

[[noreturn]] void
parsingError(const std::string &msg) {
	throw SceneFormatException(msg);
}
[[noreturn]] void
errMissingAttrib(const char *parent, const char *attribType,
				 const char *attribName) {
	parsingError(parent + std::string(" must have a ") + attribType +
				 " attribute " + '\'' + attribName + "'.");
}

template <std::size_t N>
bool
isVec(const json11::Json &value) {
	return value.is_array() && value.array_items().size() == N &&
		   std::all_of(value.array_items().begin(), value.array_items().end(),
					   [](auto &&v) { return v.is_number(); });
}

glm::vec3
parseVec3(const json11::Json &value) {
	return glm::vec3{value.array_items()[0].number_value(),
					 value.array_items()[1].number_value(),
					 value.array_items()[2].number_value()};
}

glm::vec3
parseVec3Attrib(const json11::Json &parent, const char *parentName,
				const char *attribName) {

	auto &&vec = parent[attribName];
	if (!isVec<3>(vec))
		errMissingAttrib(parentName, "vec3", attribName);

	return glm::vec3{vec.array_items()[0].number_value(),
					 vec.array_items()[1].number_value(),
					 vec.array_items()[2].number_value()};
}

float
parseFloatAttrib(const json11::Json &parent, const char *parentName,
				 const char *attribName) {

	auto &&f = parent[attribName];
	if (!f.is_number())
		errMissingAttrib(parentName, "float", attribName);

	return f.number_value();
}

std::string
parseStringAttrib(const json11::Json &parent, const char *parentName,
				  const char *attribName) {
	auto &&maybeStr = parent[attribName];
	if (!maybeStr.is_string())
		errMissingAttrib(parentName, "string", attribName);
	else
		return maybeStr.string_value();
}
void
parseCamera(Scene &scene, const json11::Json &camera) {

	auto &&pos = parseVec3Attrib(camera, "Camera", "position");
	auto &&targetPos = parseVec3Attrib(camera, "Camera", "targetPos");
	auto &&up = parseVec3Attrib(camera, "Camera", "up");

	scene.getCam().lookAt(pos, targetPos, up);
}
void
parsePointLight(Scene &scene, const json11::Json &pointLight,
				const std::string &name, const glm::vec3 &color,
				float strength) {
	auto &&pos = parseVec3Attrib(pointLight, name.c_str(), "position");

	scene.getPointLights().emplace_back(name, pos, strength, color);
}
void
parseAreaLight(Scene &scene, const json11::Json &areaLight,
			   const std::string &name, const glm::vec3 &color,
			   float strength) {
	auto &&pos = parseVec3Attrib(areaLight, name.c_str(), "position");
	auto &&normal = parseVec3Attrib(areaLight, name.c_str(), "normal");
	auto &&halfSize = parseFloatAttrib(areaLight, name.c_str(), "halfSize");
	scene.getAreaLights().emplace_back(name, pos, halfSize, normal, color,
									   strength);
}
void
parseSphereLight(Scene &scene, const json11::Json &sphereLight,
				 const std::string &name, const glm::vec3 &color,
				 float strength) {
	auto &&pos = parseVec3Attrib(sphereLight, name.c_str(), "position");
	auto &&radius = parseFloatAttrib(sphereLight, name.c_str(), "radius");

	scene.getSphereLights().emplace_back(name, pos, radius, color, strength);
}
void
parseLight(Scene &scene, const json11::Json &light) {
	auto &&name = parseStringAttrib(light, "Light", "name");
	auto &&type = parseStringAttrib(light, name.c_str(), "type");
	auto &&color = parseVec3Attrib(light, name.c_str(), "color");

	auto &&strength = parseFloatAttrib(light, name.c_str(), "strength");

	if (type == "point")
		parsePointLight(scene, light, name, color, strength);
	else if (type == "area")
		parseAreaLight(scene, light, name, color, strength);
	else if (type == "sphere")
		parseSphereLight(scene, light, name, color, strength);
	else
		parsingError(
			"A light's type must be 'area', 'sphere' or 'point' string.");
}
void
parseLights(Scene &scene, const json11::Json &lights) {
	for (auto &&light : lights.array_items()) {
		if (!light.is_object())
			parsingError("Lights must be JSON objects.");
		parseLight(scene, light);
	}
}

void
parseEnvMap(Scene &scene, const json11::Json &envMap) {
	try {
		scene.setHDREnvironmentMap(envMap.string_value());

		auto &&maybeIntensity = envMap["envMapIntensity"];
		if (maybeIntensity.is_number())
			scene.getEnvMap().intensity = maybeIntensity.number_value();
	} catch (const Exception &e) {
		parsingError(e.what());
	}
}
std::string
readFile(const std::string &filename) {
	std::ifstream in(filename);
	if (in.bad())
		parsingError("Could not read '" + filename + "' file.");
	return std::string((std::istreambuf_iterator<char>(in)),
					   std::istreambuf_iterator<char>());
}
void
parseBRDFs(Scene &scene, const json11::Json &brdfs) {
	for (auto &&brdf : brdfs.array_items()) {
		auto &&name = parseStringAttrib(brdf, "BRDF", "name");
		auto &&filename = parseStringAttrib(brdf, name.c_str(), "filename");

		std::string source = readFile(filename);
		if (!scene.createBRDF(name, std::move(source)))
			parsingError("BRDF names must be unique, '" + name +
						 "' is already present.");
	}
}

void
parseMaterials(Scene &scene, const json11::Json &materials) {
	for (auto &&material : materials.array_items()) {
		auto &&name = parseStringAttrib(material, "Material", "name");
		auto &&brdfName = parseStringAttrib(material, name.c_str(), "BRDF");

		auto maybeBRDFHandle = scene.getBRDF(brdfName);
		if (!maybeBRDFHandle)
			parsingError("Material has unknown BRDF '" + brdfName + '\'');

		if (!scene.createMaterial(name, *maybeBRDFHandle))
			parsingError("Material names must be unique, '" + name +
						 "' is already present.");
	}
}
void
parseSphere(Scene &scene, const json11::Json &sphere, const std::string &name,
			const MaterialCHandle &mat) {

	auto &&radius = parseFloatAttrib(sphere, name.c_str(), "radius");
	auto &&center = parseVec3Attrib(sphere, name.c_str(), "center");

	scene.getSpheres().emplace_back(name, mat, center, radius);
}
void
parsePlane(Scene &scene, const json11::Json &plane, const std::string &name,
		   const MaterialCHandle &mat) {
	auto &&point = parseVec3Attrib(plane, name.c_str(), "position");
	auto &&normal = parseVec3Attrib(plane, name.c_str(), "normal");

	scene.getPlanes().emplace_back(name, mat, point, normal);
}
void
parseCuboid(Scene &scene, const json11::Json &cuboid, const std::string &name,
			const MaterialCHandle &mat) {
	auto &&cornerP = parseVec3Attrib(cuboid, name.c_str(), "cornerPoint");
	auto &&dims = parseVec3Attrib(cuboid, name.c_str(), "dims");
	float pitch = parseFloatAttrib(cuboid, name.c_str(), "pitch");
	float yaw = parseFloatAttrib(cuboid, name.c_str(), "yaw");
	float roll = parseFloatAttrib(cuboid, name.c_str(), "roll");

	scene.getCuboids().emplace_back(
		name, mat, cornerP, dims,
		glm::vec3{glm::radians(yaw), glm::radians(pitch), glm::radians(roll)});
}
void
parseMesh(Scene &scene, const json11::Json &mesh, const std::string &name,
		  const MaterialCHandle &mat) {
	auto &&filename = parseStringAttrib(mesh, "Mesh", "filename");
	auto &&position = parseVec3Attrib(mesh, "Mesh", "position");
	auto &&triangles = TriangleMesh::loadModel(filename.c_str(), position);
	scene.getMeshes().emplace_back(name, mat, std::move(triangles));
}

void
parseObjecs(Scene &scene, const json11::Json &objects) {

	for (auto &&object : objects.array_items()) {
		auto &&name = parseStringAttrib(object, "Object", "name");
		auto &&type = parseStringAttrib(object, name.c_str(), "type");
		auto &&mat = parseStringAttrib(object, name.c_str(), "material");

		auto &&maybeMat = scene.getMaterial(mat);
		if (!maybeMat)
			parsingError("Object has unknown material '" + mat + "'.");
		auto &&typeStr = type;
		auto &&nameStr = name;
		if (typeStr == "sphere")
			parseSphere(scene, object, nameStr, *maybeMat);
		else if (typeStr == "plane")
			parsePlane(scene, object, nameStr, *maybeMat);
		else if (typeStr == "cuboid")
			parseCuboid(scene, object, nameStr, *maybeMat);
		else if (typeStr == "mesh")
			parseMesh(scene, object, nameStr, *maybeMat);
		else
			parsingError("Object has unknown type '" + typeStr + "'.");
	}
}
} // namespace

Scene
loadSceneFromFile(const std::string &filename) {
	std::ifstream in(filename);
	std::string file((std::istreambuf_iterator<char>(in)),
					 std::istreambuf_iterator<char>());
	std::string err;
	auto json = json11::Json::parse(file, err, json11::JsonParse::COMMENTS);

	// Find camera
	if (!json.is_object())
		parsingError(
			"The outer most value must be a JSON object(list with camera, "
			"lights, objects). See example scene.");

	auto &&camera = json["camera"];
	auto &&lights = json["lights"];
	auto &&envMap = json["envMap"];
	auto &&brdfs = json["BRDFs"];
	auto &&materials = json["materials"];
	auto &&objects = json["objects"];

	if (!camera.is_object())
		parsingError("The scene must contain 'camera' object.");
	if (!lights.is_array())
		parsingError("The scene must contain 'lights' array.");
	if (!envMap.is_null() && !envMap.is_string())
		parsingError("The environment map must be string attribute.");
	if (!brdfs.is_array())
		parsingError("The scene must contain 'BRDfs' array.");
	if (!materials.is_array())
		parsingError("The scene must contain 'materials' array.");
	if (!objects.is_array())
		parsingError("The scene must contain 'objects' array.");
	Scene newScene;
	parseCamera(newScene, camera);
	parseLights(newScene, lights);
	parseBRDFs(newScene, brdfs);
	parseMaterials(newScene, materials);
	parseObjecs(newScene, objects);
	if (!envMap.is_null())
		parseEnvMap(newScene, envMap);
	return newScene;
}
} // namespace brdfEditor::clRenderer
