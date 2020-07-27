#include "Renderer.hpp"

#include <array>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>

#include <src/Core/WindowContext.hpp>
#include <src/OpenCLRenderer/CLHeaders.hpp>
#include <src/OpenCLRenderer/RendererExceptions.hpp>
#include <src/OpenCLRenderer/Scene.hpp>
#include <src/OpenCLRenderer/Kernel/ToneMappingKernelSource.hpp>

namespace brdfEditor::clRenderer {

namespace {
constexpr const char *kernelSource = "Kernels/pathTracerDL.cl";
constexpr const char *kernelFncName = "render";

const std::vector<ToneMappingKernelSource> TMsources{
	makeSimpleTMKernelSource(), makeGammaCompressionTMKernelSource()};

// Returns context properties OpenGL-interop context for passed platform.
std::array<cl_context_properties, 7>
genInteropContextProps(const WindowContext &winContext,
					   const cl::Platform &clP) {
	return {CL_GL_CONTEXT_KHR,
			(cl_context_properties)winContext.getGLContext(),
			CL_WGL_HDC_KHR,
			(cl_context_properties)winContext.getDisplayContext(),
			CL_CONTEXT_PLATFORM,
			(cl_context_properties)(clP()), // OpenCL platform
			0};                             // Null-terminated
}
// Returns context properties for general (buffered) OpenCL context for passed
// platform.
std::array<cl_context_properties, 3>
genBufferedContextProps(const cl::Platform &clP) {
	return {CL_CONTEXT_PLATFORM,
			(cl_context_properties)(clP()), // OpenCL platform
			0};                             // Null-terminated
}
// --KernelType: Derives from Kernel.
template <typename KernelType>
std::unique_ptr<KernelType>
buildKernel(const cl::Context &context, const cl::Device &dev,
			KernelMainSource &&source,
			const std::vector<ToneMappingKernelSource> &TMsources) {
	static_assert(std::is_base_of<Kernel, KernelType>(),
				  "KernelType must derive from Kernel abstract class");

	return std::make_unique<KernelType>(context, dev, std::move(source),
										TMsources);
}
bool
checkOpenCLVersion(const std::string &clVersion) {
	// Format is guaranteed to be
	// "OpenCL<space><major_version.minor_version><space><vendor-specific
	// information>"
	// https://www.khronos.org/registry/OpenCL/sdk/1.1/docs/man/xhtml/clGetDeviceInfo.html
	int major = clVersion[7] - '0';
	int minor = clVersion[9] - '0';
	return major >= 1 || minor >= 2;
}
bool
checkGLSharingExt(const std::string &extensions) {
	return extensions.find("cl_khr_gl_sharing") != std::string::npos;
}
// Returns available devices for given platform.
// Does NOT set Device::platform pointers.
std::vector<DeviceInfo>
queryDevices(const cl::Platform &platform) {
	std::vector<cl::Device> clDevices;
	platform.getDevices(CL_DEVICE_TYPE_ALL, &clDevices);
	std::vector<DeviceInfo> devices(clDevices.size());
	std::transform(clDevices.begin(), clDevices.end(), devices.begin(),
				   [&clP = platform](const cl::Device &clD) {
					   DeviceInfo dInfo;

					   dInfo.clDevice = clD;
					   dInfo.clPlatform = clP;
					   dInfo.platform = nullptr; // Don't link yet, would
												 // prevent moving the vector of
												 // platforms around.
					   dInfo.name = clD.getInfo<CL_DEVICE_NAME>();
					   dInfo.type =
						   clD.getInfo<CL_DEVICE_TYPE>() == CL_DEVICE_TYPE_GPU
							   ? DeviceInfo::GPU
							   : DeviceInfo::CPU;
					   dInfo.clVersion = clD.getInfo<CL_DEVICE_VERSION>();
					   dInfo.versionOK = checkOpenCLVersion(dInfo.clVersion);
					   dInfo.glSharingExt = checkGLSharingExt(
						   clD.getInfo<CL_DEVICE_EXTENSIONS>());
					   return dInfo;
				   });
	return devices;
}
// Returns devices that support interop with passed GL context.
std::vector<cl_device_id>
queryInteropDevicesSupport(const WindowContext &winContext,
						   const cl::Platform &clP) {
	// If anything fails, return empty vector
	// - might fail because OpenGL context and passed CL platform are on
	// distinct HW or interop is not supported.
	try {
		size_t size;
		// TODO change macro for other platforms
		assert(winContext.getDisplayType() == WindowContext::WGL);
		auto properties = genInteropContextProps(winContext, clP);

		cl_int retCode;
		/*
		// Number of compatible devices with current OpenGL context
		retCode = clGetGLContextInfoKHR(properties.data(),
										CL_DEVICES_FOR_GL_CONTEXT_KHR, 0,
										nullptr, &size);
		if (retCode != CL_SUCCESS)
			throw cl::Error(retCode, "Could not get context info.");
		if (size == 0)
			throw cl::Error(retCode, "No compatible devices");
		// Get the compatible devices
		std::vector<cl_device_id> devs(size / sizeof(cl_device_id));
		retCode = clGetGLContextInfoKHR(properties.data(),
										CL_DEVICES_FOR_GL_CONTEXT_KHR, size,
										devs.data(), nullptr);
		if (retCode != CL_SUCCESS)
			throw cl::Error(retCode, "Could not get context info.");
		*/
		//Even reporting only the GL device, the app crashes on Intel...
		cl_device_id glDevice;
		clGetGLContextInfoKHR(properties.data(),
			CL_CURRENT_DEVICE_FOR_GL_CONTEXT_KHR, sizeof(cl_device_id),
			&glDevice, nullptr);
		
		return { glDevice };
	} catch (const cl::Error &) {
		return {};
	}
}
// Returns available platform for this computer.
// --winContext: Context of the main window used for scene rendering.
//				 Is needed to setup GL interop if it is supported.
std::vector<PlatformInfo>
queryPlatforms(const WindowContext &winContext) {
	std::vector<cl::Platform> clPlatforms;
	cl::Platform::get(&clPlatforms);

	std::vector<PlatformInfo> platforms(clPlatforms.size());
	std::transform(
		clPlatforms.begin(), clPlatforms.end(), platforms.begin(),
		[&winContext = winContext](const cl::Platform &clP) {
			PlatformInfo pInfo;
			pInfo.clPlatform = clP;
			pInfo.name = clP.getInfo<CL_PLATFORM_NAME>();
			pInfo.clVersion = clP.getInfo<CL_PLATFORM_VERSION>();
			pInfo.versionOK = checkOpenCLVersion(pInfo.clVersion);
			pInfo.glSharingExt =
				checkGLSharingExt(clP.getInfo<CL_PLATFORM_EXTENSIONS>());
			pInfo.devices = queryDevices(pInfo.clPlatform);
			auto suppDevs = queryInteropDevicesSupport(winContext, clP);
			for (auto &&d : pInfo.devices) {
				bool interop = (std::find(suppDevs.begin(), suppDevs.end(),
										  d.clDevice()) != suppDevs.end());
				// The case where ext==false and interop==true
				// probably won't happen...
				d.interopOK = d.glSharingExt && interop;
			}
			pInfo.interopAvail =
				std::any_of(pInfo.devices.begin(), pInfo.devices.end(),
							[](auto &&d) { return d.interopOK; });
			return pInfo;
		});
	return platforms;
}
// Links Device::platform pointers to their platforms
void
setPlatformPointersForDevices(std::vector<PlatformInfo> &platforms) {
	for (auto &p : platforms)
		for (auto &&d : p.devices)
			d.platform = &p;
}
} // namespace

Renderer::Renderer(const WindowContext *winContext, const Scene &scene,
				   GLuint textureID, std::size_t textureWidth,
				   std::size_t textureHeight) :
	winContext(winContext),
	scene(&scene),
	textureID(textureID),
	textureWidth(textureWidth),
	textureHeight(textureHeight),
	currentDevice(nullptr),
	milisecondsElapsed(0),
	maxBounces(3),
	kernelSourceFilename(kernelSource) {
	bool init = cloadInit();
	if (!init) // TODO exception instead
		throw Exception("Could not load the OpenGL library. Please make sure that valid opencl32.dll is present on the PATH.");
	platforms = queryPlatforms(*winContext);
	setPlatformPointersForDevices(platforms);
}
Renderer::~Renderer() {
	// RESOLVE cloadTerminate() should be called after all the OpenCL resources
	// are released
	//	- put this into a struct with ctor calling cloadInit, dtor calling
	// cloadTerminate and put it as first member variable
}
void
Renderer::recompile(const std::size_t *texWidth, const std::size_t *texHeight,
					const GLuint *textureID, const Scene *scene) {
	bool widthChanged = texWidth && (textureWidth != *texWidth);
	bool heightChanged = texHeight && (textureHeight != *texHeight);
	bool texChanged = textureID && (this->textureID != *textureID);

	if (widthChanged || heightChanged || texChanged) {

		requestRecompilation(RecompilationFlags::Texture);
		if (widthChanged)
			textureWidth = *texWidth;
		if (heightChanged)
			textureHeight = *texHeight;
		if (texChanged)
			this->textureID = *textureID;
	}
	if ((scene && this->scene != scene)) {
		requestRecompilation(RecompilationFlags::Full);
		this->scene = scene;
	}
	recompile();
}
void
Renderer::recompile() {
	// TODO Check strong-exception guarantee.
	if (recompFlags == RecompilationFlags::None)
		return;

	milisecondsElapsed = 0.0;
	if (!getCurrentDevice())
		throw InvalidRenderingDevice(
			"Cannot recompile the kernel without a device selected.");

	if (isRecompilationRequested(RecompilationFlags::BRDFs)) {
		requestRecompilation(RecompilationFlags::Full);

		std::ifstream kernelSourceFile{this->kernelSourceFilename};
		std::string source((std::istreambuf_iterator<char>{kernelSourceFile}),
						   std::istreambuf_iterator<char>{});

		auto &&kernelSource = KernelMainSource(std::move(source), kernelFncName,
											   scene->getBRDFs());

		if (currentDevice->interopOK)
			kernel = buildKernel<InteropKernel>(
				clContext, clDevice, std::move(kernelSource), TMsources);
		else
			kernel = buildKernel<BufferedKernel>(
				clContext, clDevice, std::move(kernelSource), TMsources);
	}
	if (isRecompilationRequested(RecompilationFlags::Texture))
		kernel->setTexture(textureWidth, textureHeight, textureID);
	if (isRecompilationRequested(RecompilationFlags::MaterialsParameters))
		kernel->setSceneMaterialParameters(*scene);
	if (isRecompilationRequested(RecompilationFlags::SceneObjects))
		kernel->setScene(*scene);
	if (isRecompilationRequested(RecompilationFlags::Camera))
		kernel->setCamera(scene->getCam());
	kernel->setMaxBounces(maxBounces);
	recompFlags = RecompilationFlags::None;
}
std::size_t
Renderer::getNumSamples() const {
	return kernel ? kernel->getNumSamples() : 0;
}
const std::size_t &
Renderer::getMilisecondsElapsed() const {
	return milisecondsElapsed;
}
void
Renderer::beginRender() {
	if (!currentDevice)
		throw InvalidRenderingDevice("No rendering device is set.");
	if (isRecompilationRequested())
		throw RecompilationNeeded(
			"Device has been changed so the kernel must be recompiled.");

	renderStart = std::chrono::steady_clock::now();
	kernel->beginExecute(clQueue);
}

void
Renderer::finishRender() {
	using namespace std::chrono;
	assert(kernel);
	kernel->finishExecute(clQueue);
	milisecondsElapsed += static_cast<size_t>(
		duration_cast<milliseconds>(steady_clock::now() - renderStart).count());
}

std::shared_ptr<CustomKernel>
Renderer::genCustomKernel(const std::string &source,
						  const std::string &fncName) {
	cl::Program cusProgram;
	try {
		cusProgram = cl::Program{clContext, source, false};
		cusProgram.build("-I Kernels/");
	} catch (const cl::Error &) {
		std::string str =
			cusProgram.getBuildInfo<CL_PROGRAM_BUILD_LOG>(clDevice);
		throw Exception(str);
	}
	try {
		auto cusKernel = cl::Kernel(cusProgram, fncName.c_str());
		auto ptr = std::make_shared<CustomKernel>(
			PassKey<Renderer>{}, clContext, clQueue, std::move(cusKernel),
			std::move(cusProgram));
		validCustomKernels.push_back(ptr);
		return ptr;
	} catch (const cl::Error &) {
		throw Exception("Kernel with the name '" + fncName +
						"' does not exists.");
	}
}

std::size_t
Renderer::getTextureWidth() const {
	return textureWidth;
}
std::size_t
Renderer::getTextureHeight() const {
	return textureHeight;
}
GLuint
Renderer::getTextureID() const {
	return textureID;
}
const DeviceInfo *
Renderer::getCurrentDevice() const {
	return currentDevice;
}
void
Renderer::setDevice(const DeviceInfo *device) {
	if (device == currentDevice)
		return;
	requestRecompilation(RecompilationFlags::Full);
	// Invalidate all kernels as the device&context they were using will be
	// destroyed.
	for (auto &&customKernelWeak : validCustomKernels) {
		if (auto &&customKernel = customKernelWeak.lock())
			customKernel->setValidity({}, false);
	}
	validCustomKernels.clear();
	currentDevice = device;
	// LOG
	std::cout << "Settings OpenCL device to: ";
	if (!currentDevice) {
		std::cout << "None\n";
		return;
	}
	assert(currentDevice->versionOK);
	std::cout << currentDevice->name << '\n';
	clDevice = currentDevice->clDevice;

	if (currentDevice->interopOK) {
		auto properties =
			genInteropContextProps(*winContext, device->clPlatform);
		clContext = cl::Context(clDevice, properties.data());
	} else {
		auto properties = genBufferedContextProps(device->clPlatform);
		clContext = cl::Context(clDevice, properties.data());
	}

	clQueue = cl::CommandQueue(clContext, clDevice, 0);
}

void
Renderer::requestRecompilation(Renderer::RecompilationFlags flags) {
	recompFlags |= flags;
}

Renderer::RecompilationFlags
Renderer::requestedRecompilation() const {
	return recompFlags;
}

bool
Renderer::isRecompilationRequested() const {
	return requestedRecompilation() != RecompilationFlags::None;
}

bool
Renderer::isRecompilationRequested(RecompilationFlags flag) const {
	return (flag & requestedRecompilation()) == flag;
}

const Kernel::TMKernels_t &
Renderer::getToneMappers() const {
	return kernel->getToneMappers();
}

void
Renderer::selectToneMapper(std::size_t index) {
	kernel->selectToneMapper(index);
}

void
Renderer::switchKernel(const char *filename) {
	this->kernelSourceFilename = filename;
	this->requestRecompilation(RecompilationFlags::BRDFs);
}

void
Renderer::setMaxBounces(std::size_t maxBounces) {
	if (this->maxBounces != maxBounces)
		this->requestRecompilation(RecompilationFlags::MaterialsParameters);
	this->maxBounces = maxBounces;
}

std::size_t
Renderer::getMaxBounces() const {
	return maxBounces;
}

std::size_t
Renderer::getSelectedToneMapperI() const {
	return kernel->getSelectedToneMapperI();
}

const std::vector<PlatformInfo> &
Renderer::getAvailablePlatforms() const {
	return platforms;
}

Renderer::RecompilationFlags
operator|(Renderer::RecompilationFlags l, Renderer::RecompilationFlags r) {
	using E = Renderer::RecompilationFlags;
	using T = std::underlying_type_t<E>;
	return static_cast<E>(static_cast<T>(l) | static_cast<T>(r));
}

Renderer::RecompilationFlags operator&(Renderer::RecompilationFlags l,
									   Renderer::RecompilationFlags r) {
	using E = Renderer::RecompilationFlags;
	using T = std::underlying_type_t<E>;
	return static_cast<E>(static_cast<T>(l) & static_cast<T>(r));
}

Renderer::RecompilationFlags &
operator|=(Renderer::RecompilationFlags &l, Renderer::RecompilationFlags r) {
	using E = Renderer::RecompilationFlags;
	using T = std::underlying_type_t<E>;
	return l = static_cast<E>(static_cast<T>(l) | static_cast<T>(r));
}

Renderer::RecompilationFlags &
operator&=(Renderer::RecompilationFlags &l, Renderer::RecompilationFlags r) {
	using E = Renderer::RecompilationFlags;
	using T = std::underlying_type_t<E>;
	return l = static_cast<E>(static_cast<T>(l) & static_cast<T>(r));
}

} // namespace brdfEditor::clRenderer
