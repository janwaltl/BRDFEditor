#ifndef BRDFEDITOR_OPENCL_PLANE_HEADER
#define BRDFEDITOR_OPENCL_PLANE_HEADER

#include <src/OpenCLRenderer/CLHeaders.hpp>
#include <glm/glm.hpp>

#include "Object.hpp"

namespace brdfEditor::clRenderer {

	class Plane : public Object {
	public:
		struct clRep {
			cl_float4 center;
			cl_float4 normal;
		};
		Plane(const std::string &name, const MaterialCHandle &mat,
			const glm::vec3 &center, const glm::vec3& normal);

		clRep
			getCL() const;

	private:
		glm::vec3 center;
		glm::vec3 normal;
	};
} // namespace brdfEditor::clRenderer
#endif // !BRDFEDITOR_OPENCL_PLANE_HEADER

