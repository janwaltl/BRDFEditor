#ifndef BRDFEDITOR_OPENCL_CUBOID_HEADER
#define BRDFEDITOR_OPENCL_CUBOID_HEADER

#include <src/OpenCLRenderer/CLHeaders.hpp>
#include <glm/glm.hpp>

#include "Object.hpp"

namespace brdfEditor::clRenderer {

class Cuboid : public Object {
public:
	struct clRep {
		// Corner point + extents
		// p.w is unused
		// n_.xyz = normal
		// n_.w = length
		cl_float4 p, nX, nY, nZ;
	};
	//--yawPitchRoll: Specifies angles for rotation around thes axes
	//  yaw=(0,1,0), pitch=(1,0,0), roll(0,0,1). Angles in radians.
	Cuboid(const std::string &name, const MaterialCHandle &mat,
		   const glm::vec3 &corner, const glm::vec3 &sizes,
		   const glm::vec3 &yawPitchRoll);

	clRep
	getCL() const;

private:
	glm::vec3 cornerP;
	glm::vec3 nX, nY, nZ;
	glm::vec3 dims;
};
} // namespace brdfEditor::clRenderer
#endif // !BRDFEDITOR_OPENCL_CUBOID_HEADER
