#ifndef BRDFEDITOR_OPENCL_SPHERE_LIGHT_HEADER
#define BRDFEDITOR_OPENCL_SPHERE_LIGHT_HEADER

#include <glm/vec3.hpp>

#include <src/OpenCLRenderer/CLHeaders.hpp>

#include "Light.hpp"

namespace brdfEditor {
namespace clRenderer {
class SphereLight:public Light {
public:
	struct SphereLight_cl {
		cl_float4 centerRadius;
		cl_float4 colorIntensity;
	};
	using clRep = SphereLight_cl;
	SphereLight(const std::string& name, const glm::vec3 &pos, float radius, glm::vec3 color,
				float intensity);

	// Returns CL representation of this light
	clRep
	getCL() const;
public:
	glm::vec3 center;
	glm::vec3 color;
	float radius;
	float intensity;
};
} // namespace clRenderer
} // namespace brdfEditor
#endif // !BRDFEDITOR_OPENCL_SPHERE_LIGHT_HEADER
