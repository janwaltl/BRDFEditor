#ifndef BRDFEDITOR_OPENCL_AREA_LIGHT_HEADER
#define BRDFEDITOR_OPENCL_AREA_LIGHT_HEADER

#include <glm/vec3.hpp>

#include <src/OpenCLRenderer/CLHeaders.hpp>

#include "Light.hpp"

namespace brdfEditor {
namespace clRenderer {
class AreaLight : public Light {
public:
	struct AreaLight_cl {
		// center.xyz = center
		// center.w = x,y,z half size ("radius")
		cl_float4 centerSize;
		//.w unused
		cl_float4 normal;
		//.xyz = RGB
		//.w intensity
		cl_float4 colorIntensity;
	};
	using clRep = AreaLight_cl;

	AreaLight(const std::string &name, const glm::vec3 &pos, float halfSize,
			  const glm::vec3 &normal, glm::vec3 color, float intensity);

	// Returns CL representation of this light
	clRep
	getCL() const;
	// TODO Get/set for GUI?
public:
	glm::vec3 pos, normal, col;
	float halfSize;
	float intensity;
};
} // namespace clRenderer
} // namespace brdfEditor
#endif // !BRDFEDITOR_OPENCL_AREA_LIGHT_HEADER
