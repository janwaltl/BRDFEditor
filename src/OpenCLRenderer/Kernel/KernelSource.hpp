#ifndef BRDFEDITOR_OPENCL_KERNEL_SOURCE_HEADER
#define BRDFEDITOR_OPENCL_KERNEL_SOURCE_HEADER

#include <map>
#include <string>
#include <vector>

#include "src/OpenCLRenderer/Materials/material.hpp"

namespace brdfEditor::clRenderer {
class BRDF;
// Contains source code for an OpenCL kernel program with the main kernel
// function.
class KernelMainSource final {
public:
	// Creates the OpenCL kernel source from main kernel entry function and list
	// of BRDF functions. Also creates BRDFEval and BRDFSampleEval functions
	// that evalutes the materials 	kernelFncSource - must contain __kernel
	// function
	// with 'kernelFncName' + any helper functions, macros...
	KernelMainSource(const std::string &kernelFncSource,
					 std::string kernelFncName, const std::vector<BRDFsCHandle> &BRDFs);

	const std::string &
	getKernelFncName() const;
	// Returns ID fo reach BRDF that was passed in the ctor.
	// Is used for matID for the BRDF switch function.
	int
	getMatID(const std::string &brdfFncName) const;
	const std::string &
	getSource() const;

private:
	std::string source;
	std::map<std::string, int> matIDs; // TODO rename to brdfID
	std::string kernelFncName;
};
} // namespace brdfEditor::clRenderer
#endif // BRDFEDITOR_OPENCL_KERNEL_SOURCE_HEADER
