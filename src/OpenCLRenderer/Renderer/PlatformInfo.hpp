#ifndef BRDFEDITOR_OPENCL_RENDERER_RENDERER_PLATFORM_INFO_HEADER
#define BRDFEDITOR_OPENCL_RENDERER_RENDERER_PLATFORM_INFO_HEADER

#include <vector>
#include <string>

#include <src/OpenCLRenderer/CLHeaders.hpp>

#include "DeviceInfo.hpp"

namespace brdfEditor::clRenderer {
// Information about an available OpenCL platform.
struct PlatformInfo {
	std::string name;
	// OpenCL version>=1.2
	bool versionOK;
	// If the platform supports cl_khr_gl_sharing extension.
	bool glSharingExt;
	// If there's a device that support interop with OpenGL.
	bool interopAvail;
	// Format OpenCL<space><major_version.minor_version><space><vendor-specific
	// information>
	// https://www.khronos.org/registry/OpenCL/sdk/1.1/docs/man/xhtml/clGetDeviceInfo.html
	std::string clVersion;
	std::vector<DeviceInfo> devices;
	cl::Platform clPlatform;
};
} // namespace brdfEditor::clRenderer
#endif // !BRDFEDITOR_OPENCL_RENDERER_RENDERER_PLATFORM_INFO_HEADER
