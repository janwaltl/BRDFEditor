#include "Kernel.hpp"

#include <algorithm>
#include <numeric>
#include <random>
#include <cload/cload.h>
#include <external/stb/stb_image.h>
#include <src/Core/Exception.hpp>
#include <src/OpenclRenderer/Materials/material.hpp>
#include <src/OpenCLRenderer/Lights/AreaLight.hpp>
#include <src/OpenCLRenderer/Lights/PointLight.hpp>
#include <src/OpenCLRenderer/Objects/Sphere.hpp>
#include <src/OpenCLRenderer/Scene.hpp>

#include "ToneMappingKernel.hpp"

namespace brdfEditor::clRenderer {
namespace {
constexpr const std::uint64_t rngSeed = 123;

struct EnvMapBuffers {
	cl::Image1D envMarginalRowHist;
	cl::Image2D envRowHists;
	cl::Image2D envMap;
};
EnvMapBuffers
createEnvMapBuffers(const cl::Context &context, const EnvMap_cl &envMap) {
	// Merge
	cl::ImageFormat format;
	format.image_channel_data_type = CL_FLOAT;
	format.image_channel_order = CL_RGBA;

	EnvMapBuffers envMapBuffers;

	envMapBuffers.envMarginalRowHist = cl::Image1D{
		context,
		CL_MEM_COPY_HOST_PTR | CL_MEM_READ_ONLY | CL_MEM_HOST_NO_ACCESS, format,
		envMap.envMarginalRowHist.size(),
		(void *)envMap.envMarginalRowHist.data()};

	envMapBuffers.envMap = cl::Image2D(
		context,
		CL_MEM_READ_ONLY | CL_MEM_HOST_NO_ACCESS | CL_MEM_COPY_HOST_PTR, format,
		envMap.width, envMap.height, 0, (void *)envMap.envMap.data());

	envMapBuffers.envRowHists = cl::Image2D(
		context,
		CL_MEM_READ_ONLY | CL_MEM_HOST_NO_ACCESS | CL_MEM_COPY_HOST_PTR, format,
		envMap.width, envMap.height, 0, (void *)envMap.envRowHists.data());
	return envMapBuffers;
}

template <typename clObj>
cl::Buffer
createObjectBufferFromclRep(std::vector<clObj> &clObjects,
							const cl::Context &context) {
	if (clObjects.size() == 0)
		return cl::Buffer{};

	cl_mem_flags buffFlags =
		CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR | CL_MEM_HOST_NO_ACCESS;
	return cl::Buffer{context, buffFlags,
					  clObjects.size() * sizeof clObjects[0], clObjects.data()};
}
template <typename Obj>
cl::Buffer
createObjectBuffer(const std::vector<Obj> &objs, const cl::Context &context) {
	if (objs.empty())
		return cl::Buffer{};

	std::vector<Obj::clRep> clObjects;
	clObjects.reserve(objs.size());
	for (auto &&o : objs)
		clObjects.push_back(o.getCL());

	return createObjectBufferFromclRep(clObjects, context);
}

KernelObjectsBUffers
createObjectsBuffers(const Scene &scene, const cl::Context &context,
					 std::size_t &numTriangles) {
	KernelObjectsBUffers buffers;
	buffers.pointLights = createObjectBuffer(scene.getPointLights(), context);
	buffers.areaLights = createObjectBuffer(scene.getAreaLights(), context);
	buffers.sphereLights = createObjectBuffer(scene.getSphereLights(), context);
	buffers.spheres = createObjectBuffer(scene.getSpheres(), context);
	buffers.planes = createObjectBuffer(scene.getPlanes(), context);
	buffers.cuboids = createObjectBuffer(scene.getCuboids(), context);

	BVH::clTris clTriangles;
	std::vector<TriangleMesh::TriangleMesh_cl> clMeshes;
	BVH::clNodes clBVHNodes;

	for (auto &&mesh : scene.getMeshes())
		clMeshes.push_back(mesh.getCL(clTriangles, clBVHNodes));
	numTriangles = clTriangles.size();

	buffers.meshes = createObjectBufferFromclRep(clMeshes, context);
	buffers.triangles = createObjectBufferFromclRep(clTriangles, context);
	buffers.bvhNodes = createObjectBufferFromclRep(clBVHNodes, context);

	return buffers;
}

std::pair<KernelObjectsBUffers, Scene_cl>
uploadScene(const Scene &s, const cl::Context &context) {
	std::size_t numTriangles;
	auto sceneBuffers = createObjectsBuffers(s, context, numTriangles);

	Scene_cl clScene;
	auto getNums = [](const auto &objs) {
		return static_cast<cl_int>(objs.size());
	};
	// Reset scene info
	clScene.numPointLight = getNums(s.getPointLights());
	clScene.numAreaLights = getNums(s.getAreaLights());
	clScene.numSphereLights = getNums(s.getSphereLights());
	clScene.numSpheres = getNums(s.getSpheres());
	clScene.numPlanes = getNums(s.getPlanes());
	clScene.numCuboids = getNums(s.getCuboids());
	clScene.numMeshes = getNums(s.getMeshes());

	auto &&cam_cl = s.getCam().getCL();
	clScene.camDir = cam_cl.dir;
	clScene.camPos = cam_cl.pos;
	clScene.camUp = cam_cl.up;

	return {sceneBuffers, clScene};
}
// Represents a buffer that stores all BRDFs' parameters.
struct MatParamsBuffer {
	using paramOffsets_t = std::map<std::string, std::size_t>;

	// Buffer of Param_cl structures.
	cl::Buffer matParams;
	// Map that for given BRDF's name returns the offset
	// of its first parameter stored in the matParamBuffer.
	paramOffsets_t matParamOffsets;

	MatParamsBuffer(const Scene &s, const cl::Context &context,
					const KernelMainSource &mainKernel) {
		const auto &sceneMats = s.getMaterials();

		std::size_t numParams = 0;
		for (auto &&m : sceneMats)
			numParams += m->second.params.size();

		std::vector<BRDFParam_cl> args{numParams};

		std::size_t off = 0;
		for (auto &&matHandle : sceneMats) {
			auto &&[matName, mat] = *matHandle;
			std::size_t i = 0;
			for (auto &&[paramName, param] : mat.params) {
				args[off + i++] = param.getCL();
			}
			matParamOffsets[matName] = off;
			off += mat.params.size();
		}
		if (numParams == 0)
			matParams = cl::Buffer{};
		else
			matParams =
				cl::Buffer(context, std::begin(args), std::end(args), true);
	}
};
// Buffers that store information about materials and their parameters.
struct MatInfosBuffers {
	// Buffer of MatInfo_cl structures.
	cl::Buffer infosBuffer;
	// Buffer of Param_cl structures.
	cl::Buffer paramsBuffer;
	// OpenCL struct for a material.
	struct MatInfo_cl {
		cl_int brdfID;
		cl_int paramOff;
	};
	static MatInfosBuffers
	create(const Scene &s, const cl::Context &context,
		   const KernelMainSource &mainKernel) {
		const auto &sceneMats = s.getMaterials();
		MatParamsBuffer matParams{s, context, mainKernel};

		const auto &objects = s.getObjects();
		// Number of objects in the scene, not just spheres
		std::vector<MatInfo_cl> clMatInfos;

		for (auto &&o : objects) {
			MatInfo_cl mI;
			mI.brdfID = mainKernel.getMatID(
				o->getMatHandle()->second.brdf->second.getBRDFFncName());
			mI.paramOff = static_cast<cl_int>(
				matParams.matParamOffsets[o->getMatHandle()->first]);
			clMatInfos.push_back(mI);
		}

		MatInfosBuffers buffer;
		buffer.infosBuffer = cl::Buffer(context, std::begin(clMatInfos),
										std::end(clMatInfos), true);
		buffer.paramsBuffer = matParams.matParams;
		return buffer;
	}
};

// Generates n uniformly distributed 64-bit values, upper 32bits are in X, lower
// in Y
std::vector<cl_uint2>
genRNGVec(std::size_t n, std::uint64_t seed) {
	std::vector<cl_uint2> rngState(n);
	std::random_device eng;
	std::uniform_int_distribution<std::uint64_t> rng;
	for (std::size_t i = 0; i < rngState.size(); ++i) {
		std::uint64_t val = rng(eng);
		rngState[i].x = val >> 32ULL;
		rngState[i].y = val & 0xFFFFFFFF;
	}
	return rngState;
}
// --rngState: Is not modified but must be non-const because cl::Buffer requires
// void* to data.
cl::Buffer
createRNGStateBuffer(std::vector<cl_uint2> &rngState,
					 const cl::Context &context) {
	// IMPROVE Can be further sped up by doing this copy using non-blocking
	// queue.enqueueWriteBuffer
	return cl::Buffer(context,
					  CL_MEM_READ_ONLY | CL_MEM_HOST_NO_ACCESS |
						  CL_MEM_COPY_HOST_PTR,
					  rngState.size() * sizeof(cl_uint2), rngState.data());
}

// Appends the source of passed TM kernel to the main kernel source.
// Returns: Modified TMsource.getName() to hopefully make it unique inside the
// main kernel.
std::string
appendToneMapper(std::string &mainSource,
				 const ToneMappingKernelSource &TMsource) {

	// For making the TMNames unique
	std::size_t i = 0;

	std::string source{TMsource.getSource()};

	const std::string tonemapping = "TONE_MAPPING";
	auto fncNameBeginIT = source.find("TONE_MAPPING");
	// Ensured by ToneMappingKernelSource's constructor.
	assert(fncNameBeginIT != source.npos);

	// To make them unique and resolve any clashes with the main source.
	std::string newName{TMsource.getName() + "_" +
						std::to_string(mainSource.length() + i)};

	source.replace(fncNameBeginIT, tonemapping.length(), newName);
	mainSource += '\n';
	mainSource += source;
	return newName;
}
} // namespace

Kernel::Kernel(const cl::Context &context, const cl::Device &dev,
			   KernelMainSource kernelSource,
			   const std::vector<ToneMappingKernelSource> &TMsources) :
	context(context),
	kernelSource(std::move(kernelSource)),
	device(dev),
	selectedTMKernelI(0),
	maxBounces(5) {

	std::string source = this->kernelSource.getSource();
	std::vector<std::string> newNames;

	for (auto &&TMSource : TMsources)
		newNames.emplace_back(appendToneMapper(source, TMSource));

	try {
		program = cl::Program{this->context, source, false};
		program.build("-I Kernels/");
	} catch (const cl::Error &e) {
		std::string str = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device);
		throw KernelException("Kernel build error(" + std::to_string(e.err()) +
							  "): \n" + str);
	}

	kernel = cl::Kernel(program, this->kernelSource.getKernelFncName().c_str());

	assert(TMsources.size() == newNames.size());
	for (std::size_t i = 0; i < TMsources.size(); ++i)
		TMkernels.emplace_back(TMsources[i].generateKernel(
			cl::Kernel(program, newNames[i].c_str())));
}
void
Kernel::setArguments(std::size_t texWidth, std::size_t texHeight,
					 GLuint textureID, const Scene &scene,
					 std::size_t maxBounces) {
	setScene(scene);
	setTexture(texWidth, texHeight, textureID);
	setSceneMaterialParameters(scene);
	setMaxBounces(maxBounces);
}
void
Kernel::setScene(const Scene &scene) {
	// Reuploads the scene
	std::tie(sceneBuffers, this->scene) = uploadScene(scene, context);
	this->setCamera(scene.getCam());

	kernel.setArg(0, sceneBuffers.spheres);
	kernel.setArg(1, sceneBuffers.planes);
	kernel.setArg(2, sceneBuffers.cuboids);
	kernel.setArg(3, sceneBuffers.bvhNodes);
	kernel.setArg(4, sceneBuffers.triangles);
	kernel.setArg(5, sceneBuffers.meshes);
	kernel.setArg(6, sceneBuffers.pointLights);
	kernel.setArg(7, sceneBuffers.areaLights);
	kernel.setArg(8, sceneBuffers.sphereLights);
	kernel.setArg(9, this->scene);
	// Not setting 8, numSamples now, will be set by execute()

	auto &map = scene.getEnvMap();

	auto buffers = createEnvMapBuffers(context, map.getCL());
	kernel.setArg(14, envMap.marginalHist = buffers.envMarginalRowHist);
	kernel.setArg(15, envMap.rowHists = buffers.envRowHists);
	kernel.setArg(16, envMap.values = buffers.envMap);
	kernel.setArg(17, map.intensity);
}
void
Kernel::setSceneMaterialParameters(const Scene &scene) {
	numSamples = 0;

	MatInfosBuffers matInfoBuffers =
		MatInfosBuffers::create(scene, context, kernelSource);
	matInfos = matInfoBuffers.infosBuffer;
	matParams = matInfoBuffers.paramsBuffer;

	kernel.setArg(10, matInfos);
	kernel.setArg(11, matParams);
}
void
Kernel::setTexture(std::size_t texWidth, std::size_t texHeight,
				   GLuint textureID) {
	numSamples = 0;

	std::size_t imgSize = texWidth * texHeight;
	buffImage = cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_HOST_NO_ACCESS,
						   imgSize * sizeof(cl_float4));
	texture = textureID;
	// Reset RNGs, recache if needed
	if (cachedRngVec.size() < imgSize)
		cachedRngVec = genRNGVec(imgSize, rngSeed);
	rngState = createRNGStateBuffer(cachedRngVec, context);
	outImage = this->genOutImage(context, texWidth, texHeight, textureID);
	globalWork = cl::NDRange(texWidth, texHeight);

	kernel.setArg(12, buffImage);
	kernel.setArg(13, rngState);
}
void
Kernel::setCamera(const Camera &sceneCam) {
	numSamples = 0;
	auto cam_cl = sceneCam.getCL();
	scene.camDir = cam_cl.dir;
	scene.camPos = cam_cl.pos;
	scene.camUp = cam_cl.up;
	kernel.setArg(9, this->scene);
}
void
Kernel::setMaxBounces(std::size_t maxBounces) {
	this->maxBounces = static_cast<cl_int>(maxBounces);
	kernel.setArg(19, this->maxBounces);
}
void
Kernel::beginExecute(const cl::CommandQueue &queue) {
	kernel.setArg(18, ++numSamples);
	doBeginExecute(queue);
}
void
Kernel::finishExecute(const cl::CommandQueue &queue) {
	doFinishExecute(queue);
}

cl_int
Kernel::getNumSamples() {
	return numSamples;
}

const Kernel::TMKernels_t &
Kernel::getToneMappers() const {
	return TMkernels;
}

void
Kernel::selectToneMapper(std::size_t index) {
	assert(index < TMkernels.size());
	selectedTMKernelI = index;
}

std::size_t
Kernel::getSelectedToneMapperI() const {

	return selectedTMKernelI;
}

void
Kernel::executeTMKernel(const cl::CommandQueue &queue) {
	auto &kernel = TMkernels[selectedTMKernelI];
	kernel->setArguments(numSamples, buffImage, *outImage, globalWork);
	kernel->execute(queue);
}

void
BufferedKernel::doBeginExecute(const cl::CommandQueue &queue) {
	queue.enqueueNDRangeKernel(kernel, 0, globalWork);
	executeTMKernel(queue);
	std::size_t imgSize = globalWork[0] * globalWork[1];
	imgBuff.resize(imgSize);
	cl::size_t<3> region;
	region[0] = globalWork[0];
	region[1] = globalWork[1];
	region[2] = 1; // Z must be 1 for 2D images
	queue.enqueueReadImage(*outImage, CL_FALSE, cl::size_t<3>{}, region, 0, 0,
						   imgBuff.data());
}
void
BufferedKernel::doFinishExecute(const cl::CommandQueue &queue) {
	queue.finish();
	glTextureSubImage2D(texture, 0, 0, 0, globalWork[0], globalWork[1], GL_RGBA,
						GL_FLOAT, imgBuff.data());
}
std::unique_ptr<cl::Image>
BufferedKernel::genOutImage(const cl::Context &context, std::size_t texWidth,
							std::size_t texHeight, GLuint textureID) {
	return std::make_unique<cl::Image2D>(
		context, CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY,
		cl::ImageFormat{CL_RGBA, CL_FLOAT}, texWidth, texHeight);
}
//Default   13
//1x1		171
//32x32		
void
InteropKernel::doBeginExecute(const cl::CommandQueue &queue) {
	queue.enqueueNDRangeKernel(kernel, 0, globalWork, cl::NDRange{ 16,16 });
	glFinish();
	clEnqueueAcquireGLObjects(queue(), 1, &outImage->operator()(), 0, nullptr,
							  nullptr);
	executeTMKernel(queue);
	clEnqueueReleaseGLObjects(queue(), 1, &outImage->operator()(), 0, nullptr,
							  nullptr);
}
void
InteropKernel::doFinishExecute(const cl::CommandQueue &queue) {
	auto ret = queue.finish();
	assert(ret == CL_SUCCESS);
}
std::unique_ptr<cl::Image>
InteropKernel::genOutImage(const cl::Context &context, std::size_t texWidth,
						   std::size_t texHeight, GLuint textureID) {
	return std::make_unique<cl::ImageGL>(context, CL_MEM_WRITE_ONLY,
										 GL_TEXTURE_2D, 0, texture);
}
} // namespace brdfEditor::clRenderer
