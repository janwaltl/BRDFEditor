#ifndef BRDFEDITOR_OPENCL_KERNEL_HEADER
#define BRDFEDITOR_OPENCL_KERNEL_HEADER

#include <memory>
#include <string>
#include <vector>

#include <glad/glad.h>
#include <src/Core/Exception.hpp>
#include <src/OpenCLRenderer/CLHeaders.hpp>
#include <src/OpenCLRenderer/Camera.hpp>

#include "KernelSource.hpp"
#include "ToneMappingKernel.hpp"
#include "ToneMappingKernelSource.hpp"

namespace brdfEditor::clRenderer {
class Scene;

class KernelException : public Exception {
public:
	using Exception::Exception;
};

struct KernelObjectsBUffers {
	cl::Buffer pointLights;
	cl::Buffer areaLights;
	cl::Buffer sphereLights;

	cl::Buffer spheres;
	cl::Buffer planes;
	cl::Buffer cuboids;

	cl::Buffer meshes;
	cl::Buffer triangles;
	cl::Buffer bvhNodes;

};
// Representation of the Scene in the kernel.
struct Scene_cl {
	cl_float4 camPos;
	cl_float4 camDir;
	cl_float4 camUp;
	cl_int numPointLight;
	cl_int numAreaLights;
	cl_int numSphereLights;

	cl_int numSpheres;
	cl_int numPlanes;
	cl_int numCuboids;
	cl_int numMeshes;
};

// Abstract class representing a kernel used for path tracing algorithm.
// Wrapper around cl::Program with interface that allows settings kernel's
// arguments and execute it.
class Kernel {
public:
	using TMKernels_t = std::vector<std::unique_ptr<ToneMappingKernel>>;

	// Creates kernel with given source code.
	// --contex: OpenCL context must outlive this instance.
	// --TMsources: Must contain at least one
	Kernel(const cl::Context &context, const cl::Device &dev,
		   KernelMainSource kernelSource,
		   const std::vector<ToneMappingKernelSource> &TMsources);
	virtual ~Kernel() = default;

	// Passes new arguments to the kernel.
	// --textureID: Must remain valid throughout use of these arguments.
	void
	setArguments(std::size_t texWidth, std::size_t texHeight, GLuint textureID,
				 const Scene &scene, std::size_t maxBounces);

	void
	setScene(const Scene &scene);
	void
	setSceneMaterialParameters(const Scene &scene);
	void
	setTexture(std::size_t texWidth, std::size_t texHeight, GLuint textureID);
	void
	setCamera(const Camera &sceneCam);
	void
		setMaxBounces(std::size_t maxBounces);
	void
	beginExecute(const cl::CommandQueue &queue);
	void
	finishExecute(const cl::CommandQueue &queue);

	// How many times was the scene sampled and therefore how many samples the
	// texture contains.
	cl_int
	getNumSamples();

	// Returns: List of available tone-mapping algortihms that can be applied to
	// the rendered image.
	const TMKernels_t &
	getToneMappers() const;

	// Select which TMkernel to use.
	//--index: Must be in range [0,getToneMappers().size()).
	void
	selectToneMapper(std::size_t index);
	// Returns: Index of selected tone-mapping algorithm. In range
	// [0,getToneMappers().size())
	std::size_t
	getSelectedToneMapperI() const;

protected:
	void
	executeTMKernel(const cl::CommandQueue &queue);
	cl::Kernel kernel;
	// Image that show rendered scene - buffImage averaged over the number of
	// samples.
	std::unique_ptr<cl::Image> outImage;
	// Size of work to be executed, same as size of the outImage,buffImage
	cl::NDRange globalWork;
	GLuint texture;

private:

	virtual void
	doBeginExecute(const cl::CommandQueue &queue) = 0;
	virtual void
	doFinishExecute(const cl::CommandQueue &queue) = 0;
	virtual std::unique_ptr<cl::Image>
	genOutImage(const cl::Context &context, std::size_t texWidth,
				std::size_t texHeight, GLuint textureID) = 0;

	KernelMainSource kernelSource;
	cl::Context context;
	cl::Device device;
	cl::Program program;
	// Stores material ID for reach object in the scene.
	// Serialized in the order of KernelObjectsBuffer declartion.
	cl::Buffer matInfos;
	cl::Buffer matParams;

	KernelObjectsBUffers sceneBuffers;
	Scene_cl scene;
	struct {
		cl::Image1D marginalHist;
		cl::Image2D rowHists, values;
	} envMap;
	
	// Image in which renderer stores all samples of the rendered scene
	cl::Buffer buffImage;
	// RNG generator's state for each pixel
	cl::Buffer rngState;

	// Cached vector of random numbers.
	std::vector<cl_uint2> cachedRngVec;
	cl_int numSamples;
	cl_int maxBounces;

	TMKernels_t TMkernels;
	std::size_t selectedTMKernelI;
};

// Kernel that uses a buffer to render into.
// Buffer is then copied to the texture.
class BufferedKernel final : public Kernel {
public:
	using Kernel::Kernel;
	virtual ~BufferedKernel() override = default;

private:
	std::vector<cl_float4> imgBuff;
	virtual void
	doBeginExecute(const cl::CommandQueue &queue) override final;
	virtual void
	doFinishExecute(const cl::CommandQueue &queue) override final;
	virtual std::unique_ptr<cl::Image>
	genOutImage(const cl::Context &context, std::size_t texWidth,
				std::size_t texHeight, GLuint textureID) override final;
};

// Kernel that directry renders to the OpenGL texture.
class InteropKernel final : public Kernel {
public:
	using Kernel::Kernel;

private:
	virtual void
	doBeginExecute(const cl::CommandQueue &queue) override final;
	virtual void
	doFinishExecute(const cl::CommandQueue &queue) override final;
	virtual std::unique_ptr<cl::Image>
	genOutImage(const cl::Context &context, std::size_t texWidth,
				std::size_t texHeight, GLuint textureID) override final;
};
} // namespace brdfEditor::clRenderer
#endif // ! BRDFEDITOR_OPENCL_KERNEL_HEADER
