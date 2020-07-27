#include "SphereLight.hpp"

namespace brdfEditor::clRenderer {
SphereLight::SphereLight(const std::string& name, const glm::vec3 &pos, float radius, glm::vec3 color,
						 float intensity) :
	Light(name),
	center(pos),
	color(color),
	radius(radius),
	intensity(intensity) {}

SphereLight::clRep
SphereLight::getCL() const {
	clRep rep;
	rep.centerRadius = cl_float4{center.x, center.y, center.z, radius};
	rep.colorIntensity = cl_float4{color.x, color.y, color.z, intensity};
	return rep;
}

} // namespace brdfEditor::clRenderer
