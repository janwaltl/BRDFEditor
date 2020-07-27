#include "AreaLight.hpp"
#include <glm/geometric.hpp>

namespace brdfEditor::clRenderer {

AreaLight::AreaLight(const std::string &name, const glm::vec3 &pos,
					 float halfSize, const glm::vec3 &normal, glm::vec3 color,
					 float intensity) :
	Light(name),
	pos(pos),
	halfSize(halfSize),
	normal(normal),
	col(color),
	intensity(intensity) {

	this->normal = glm::normalize(this->normal);
}
AreaLight::clRep
AreaLight::getCL() const {
	return AreaLight_cl{cl_float4{pos.x, pos.y, pos.z, halfSize},
						cl_float4{normal.x, normal.y, normal.z, 0.0f},
						cl_float4{col.r, col.g, col.b, intensity}};
}
} // namespace brdfEditor::clRenderer
