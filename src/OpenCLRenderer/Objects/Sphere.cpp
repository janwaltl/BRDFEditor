#include "Sphere.hpp"

namespace brdfEditor::clRenderer {
Sphere::Sphere(const std::string &name, const MaterialCHandle &mat,
			   const glm::vec3 &center, float radius) :
	Object(name, mat),
	center(center),
	r(radius) {}

Sphere::clRep
Sphere::getCL() const {
	return cl_float4{center.x, center.y, center.z, r};
}
} // namespace brdfEditor::clRenderer
