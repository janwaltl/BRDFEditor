#ifndef BRDFEDITOR_OPENCL_TONE_MAPPING_KERNEL_SOURCE_HEADER
#define BRDFEDITOR_OPENCL_TONE_MAPPING_KERNEL_SOURCE_HEADER

#include <functional>
#include <memory>
#include <string>

#include <src/OpenCLRenderer/CLHeaders.hpp>

namespace brdfEditor::clRenderer {

class ToneMappingKernel;
// Contains source code for a tone-mapping kernel.
// Must contain kernel function named "TONE_MAPPING" with two arguments for global float4* and global
// image2d_t. Can contain other functions but there's a risk of name clash with
// other parts of the main kernel. The string "TONE_MAPPING" must be present exaclty once - for the definiton.
class ToneMappingKernelSource {
public:

	// ToneMappingKernel(name, cl::kernel)
	// --cl::kernel: Obtained from main program in Kernel class.
	using makeKernel_fnc =
		std::function<std::unique_ptr<ToneMappingKernel>(std::string, cl::Kernel)>;

	// --source: See class description for requirements.
	ToneMappingKernelSource(std::string source, std::string name,
							makeKernel_fnc kernelGenerator);

	std::string &
	getSource();
	const std::string &
	getSource() const;
	const std::string &
	getName();
	const std::string &
	getName() const;

	std::unique_ptr<ToneMappingKernel>
	generateKernel(cl::Kernel kernel) const;
private:
	std::string source;
	std::string name;
	makeKernel_fnc kernelGenerator;
};


//Returns a source than generates 'SimpleTMKernel'
ToneMappingKernelSource makeSimpleTMKernelSource();

ToneMappingKernelSource makeGammaCompressionTMKernelSource();
} // namespace brdfEditor::clRenderer
#endif
