#ifndef BRDFEDITOR_RENDERER_CUSTOM_KERNEL_HEADER
#define BRDFEDITOR_RENDERER_CUSTOM_KERNEL_HEADER

#include <src/OpenCLRenderer/CLHeaders.hpp>
#include <src/Core/PassKey.hpp>

namespace brdfEditor::clRenderer {
class Renderer;

// Light-weight wrapper around OpenCL kernel. Created from Renderer which also
// manages its validity.
class CustomKernel {
public:
	// All parameters must come from the same Renderer instance.
	CustomKernel(PassKey<Renderer> &&key, cl::Context context,
				 cl::CommandQueue queue, cl::Kernel kernel,
				 cl::Program program);
	// Is only valid if isValid()==true.
	cl::Context &
	getContext();
	// Is only valid if isValid()==true.
	cl::Kernel &
	getKernel();
	// Is only valid if isValid()==true.
	cl::CommandQueue &
	getQueue();
	// Can only be run if isValid()==true.
	void
	run(cl::NDRange globalRange, cl::NDRange localRange = cl::NullRange);
	// Returns: Whether the kernel, associated queue and context are valid. The
	// validity is managed by a Renderer instance. They remain valid as long as
	// the renderer's device remains the same. Invalid instance must not be used and can only be replaced a valid new one.
	bool
	isValid();
	void
	setValidity(PassKey<Renderer> &&key, bool validity);

private:
	cl::Context context;
	cl::CommandQueue queue;
	cl::Kernel kernel;
	cl::Program program;
	cl::Device device;
	bool validity;
};
} // namespace brdfEditor::clRenderer
#endif // !BRDFEDITOR_RENDERER_CUSTOM_KERNEL_HEADER
