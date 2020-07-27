#ifndef BRDFEDITOR_OPENCL_POINT_LIGHT_HEADER
#define BRDFEDITOR_OPENCL_POINT_LIGHT_HEADER

#include <glm/vec3.hpp>

#include <src/OpenCLRenderer/CLHeaders.hpp>

#include "Light.hpp"

namespace brdfEditor::clRenderer {
class PointLight : public Light {
public:
	struct PointLight_cl {
		cl_float4 pos;
		cl_float4 color;
		cl_float intensity;
	};
	using clRep = PointLight_cl;

	PointLight(const std::string &name, const glm::vec3 &pos, float intensity,
			   glm::vec3 color);
	// Returns CL representation of this light
	clRep
	getCL() const;

public:
	glm::vec3 pos, col;
	float intensity;
};
} // namespace brdfEditor::clRenderer
#endif // !BRDFEDITOR_OPENCL_POINT_LIGHT_HEADER
