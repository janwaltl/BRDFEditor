#include "Plane.hpp"

namespace brdfEditor::clRenderer {
Plane::Plane(const std::string &name, const MaterialCHandle &mat,
			 const glm::vec3 &center, const glm::vec3 &normal) :
	Object(name, mat),
	center(center),
	normal(normal) {}

Plane::clRep
Plane::getCL() const {
	clRep rep;
	rep.center = cl_float4{center.x, center.y, center.z, 0.0f};
	rep.normal = cl_float4{normal.x, normal.y, normal.z, 0.0f};
	return rep;
}

} // namespace brdfEditor::clRenderer
