#ifndef BRDFEDITOR_OPENCL_RENDERER_RENDERER_DEVICE_INFO_HEADER
#define BRDFEDITOR_OPENCL_RENDERER_RENDERER_DEVICE_INFO_HEADER

#include <string>

#include <src/OpenCLRenderer/CLHeaders.hpp>

namespace brdfEditor::clRenderer {
struct PlatformInfo;

// Information about an available OpenCL device.
struct DeviceInfo {
	enum { GPU, CPU } type;
	std::string name;
	// Format OpenCL<space><major_version.minor_version><space><vendor-specific
	// information>
	// https://www.khronos.org/registry/OpenCL/sdk/1.1/docs/man/xhtml/clGetDeviceInfo.html
	std::string clVersion;
	// If the device supports cl_khr_gl_sharing extension.
	bool glSharingExt;
	// If the device support interop with OpenGL.
	bool interopOK;
	// OpenCL version>=1.2
	bool versionOK;
	// To which platform the device belongs.
	const PlatformInfo *platform;
	cl::Device clDevice;
	cl::Platform clPlatform;
};
} // namespace brdfEditor::clRenderer
#endif // !BRDFEDITOR_OPENCL_RENDERER_RENDERER_DEVICE_INFO_HEADER
