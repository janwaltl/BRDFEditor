#ifndef BRDFEDITOR_OPENCL_RENDERER_HEADER
#define BRDFEDITOR_OPENCL_RENDERER_HEADER

#include <chrono>
#include <cstddef>
#include <memory>
#include <future>

#include <src/Core/Exception.hpp>
#include <src/OpenCLRenderer/CLHeaders.hpp>
#include <src/OpenCLRenderer/Kernel/Kernel.hpp>
#include <src/OpenCLRenderer/Kernel/CustomKernel.hpp>
#include "PlatformInfo.hpp"
#include "DeviceInfo.hpp"
namespace brdfEditor {
class WindowContext;

namespace clRenderer {
class Scene;

// Renders the scene using path tracing implemented with OpenCL.
class Renderer {
public:
	//"How much" must the renderer be recompiled.
	enum class RecompilationFlags : uint8_t {
		// No need
		None = 0,
		// Only reset arguments( e.g. The scene changed)
		MaterialsParameters = 1 << 0,
		SceneObjects = 1 << 1,
		Camera = 1 << 2,
		BRDFs = 1 << 3,
		Texture = 1 << 4,
		// Full recompilation, recompiles the kernel source.
		// Needed to apply changes in active materials' BRDFs.
		Full = 0xFF,
	};

	// --winContext: Context of the main window, required for OpenCL-OpenGL
	// interop. Is captured, must outlive this instance.
	Renderer(const WindowContext *winContext, const Scene &scene,
			 GLuint textureID, std::size_t textureWidth,
			 std::size_t textureHeight);
	~Renderer();
	// (Re)sets the rendering processing with new arguments if they were passed
	// and if it was requested.
	// --textureID: OpenGL texture that remains valid for all calls to
	// begin/finishRender().
	// --texWidth,texHeight: Dimensions of the texture represented by textureID.
	// Throws: InvalidRenderingDevice if none is set.
	void
	recompile(const std::size_t *texWidth, const std::size_t *texHeight,
			  const GLuint *textureID, const Scene *scene);
	// (Re)sets the rendering processing with curent arguments if it was
	// requested.
	void
	recompile();
	// Begins to render next sampling of the scene. Renderer must have been
	// recompiled if it had been requested. Throws: InvalidRenderingDevice if
	// none is set.
	//		   RecompilationNeeded if the recompilation has been requested and
	// not done.
	void
	beginRender();
	// Finished started rendering, writes result to the texture.
	// Can ONLY be called if beginRender() returned true.
	void
	finishRender();
	// Creates an OpenCL kernel from source contained passed function name.
	// Throws: Exception if the build fails or the kernel function does not exists.
	std::shared_ptr<CustomKernel>
	genCustomKernel(const std::string &source, const std::string &fncName);
	// Number of samples rendered since last recompilation.
	std::size_t
	getNumSamples() const;
	// Time elapsed since last recompilation
	const std::size_t &
	getMilisecondsElapsed() const;
	std::size_t
	getTextureWidth() const;
	std::size_t
	getTextureHeight() const;
	GLuint
	getTextureID() const;
	// Returns: List of available tone-mapping algortihms that can be applied to
	// the rendered image.
	const Kernel::TMKernels_t &
	getToneMappers() const;
	// Select which tone-mapping algorithm will be used.
	//--index: Must be in range [0,getToneMappers().size()).
	void
	selectToneMapper(std::size_t index);
	// Saves the filename and requests the full recompilation.
	// Does NOT validate the kernel now, next recompile() may throw expection.
	void switchKernel(const char* filename);
	// Maximum number of bounces in the rendering algorithm.
	void setMaxBounces(std::size_t maxBounces);
	std::size_t getMaxBounces() const;
	//Returns: Index of selected tone-mapping algorithm. In range [0,getToneMappers().size())
	std::size_t getSelectedToneMapperI() const;
	const std::vector<PlatformInfo> &
	getAvailablePlatforms() const;
	// Returns currently used OpenCL device for rendering or nullptr.
	const DeviceInfo *
	getCurrentDevice() const;
	// Sets OpenCL device to be used for rendering.
	// --device: Nullptr to disable rendering.
	//			 pointer must point to a device from getplatforms().
	// If the device have been changed a recompilation is requested.
	void
	setDevice(const DeviceInfo *device);
	// Signals that the Renderer should be recompiled.
	void
	requestRecompilation(RecompilationFlags flags);
	RecompilationFlags
	requestedRecompilation() const;
	// Returns RecompilationFlags!=RecompilationFlags::None.
	bool
	isRecompilationRequested() const;
	bool
	isRecompilationRequested(RecompilationFlags flag) const;

private:
	const WindowContext *winContext;
	const Scene *scene;
	GLuint textureID;
	std::size_t textureWidth, textureHeight;
	std::string kernelSourceFilename;
	std::size_t maxBounces;
	std::vector<PlatformInfo> platforms;
	// Pointer into a platform inside platforms.
	const DeviceInfo *currentDevice;
	cl::Context clContext;
	cl::Device clDevice;
	cl::CommandQueue clQueue;
	std::unique_ptr<Kernel> kernel;
	std::vector<std::weak_ptr<CustomKernel>> validCustomKernels;

	std::chrono::steady_clock::time_point renderStart;
	std::size_t milisecondsElapsed;
	// Renderer must be recompiled in order to be able to render.
	//	- when device is changed.
	//	- when its requested from outside (e.g. to reflect changes in the
	// Scene).
	RecompilationFlags recompFlags;
};
Renderer::RecompilationFlags
operator|(Renderer::RecompilationFlags l, Renderer::RecompilationFlags r);
Renderer::RecompilationFlags operator&(Renderer::RecompilationFlags l,
									   Renderer::RecompilationFlags r);
Renderer::RecompilationFlags &
operator|=(Renderer::RecompilationFlags &l, Renderer::RecompilationFlags r);
Renderer::RecompilationFlags &
operator&=(Renderer::RecompilationFlags &l, Renderer::RecompilationFlags r);

} // namespace clRenderer

} // namespace brdfEditor

#endif // !BRDFEDITOR_RENDERER_HEADER
