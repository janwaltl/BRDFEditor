#ifndef BRDFEDITOR_OPENCL_SPHERE_HEADER
#define BRDFEDITOR_OPENCL_SPHERE_HEADER

#include <src/OpenCLRenderer/CLHeaders.hpp>
#include <glm/glm.hpp>

#include "Object.hpp"
namespace brdfEditor::clRenderer {
class Sphere : public Object {
public:
	using clRep = cl_float4;
	Sphere(const std::string &name, const MaterialCHandle &mat,
		   const glm::vec3 &center, float radius);

	clRep
	getCL() const;

private:
	glm::vec3 center;
	float r;
};
} // namespace brdfEditor::clRenderer
#endif // !BRDFEDITOR_OPENCL_OBJECT_HEADER
