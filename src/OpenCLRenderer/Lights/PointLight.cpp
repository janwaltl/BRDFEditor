#include "PointLight.hpp"

namespace brdfEditor::clRenderer {
PointLight::PointLight(const std::string &name,const glm::vec3 &pos, float intensity,
					   glm::vec3 color) :
	Light(name),
	pos(pos),
	col(color),
	intensity(intensity) {}
PointLight::clRep
PointLight::getCL() const {
	return PointLight_cl{cl_float4{pos.x, pos.y, pos.z, 0.0f},
						 cl_float4{col.r, col.g, col.b, 0.0f},
						 cl_float{intensity}};
}
} // namespace brdfEditor::clRenderer
