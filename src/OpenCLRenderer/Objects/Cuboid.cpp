#include "Cuboid.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

namespace brdfEditor::clRenderer {
Cuboid::Cuboid(const std::string &name, const MaterialCHandle &mat,
			   const glm::vec3 &corner, const glm::vec3 &sizes,
			   const glm::vec3 &yawPitchRoll) :
	Object(name, mat),
	cornerP(corner),
	dims(sizes) {
	auto rotMat =
		glm::eulerAngleYXZ(yawPitchRoll.x, yawPitchRoll.y, yawPitchRoll.z);

	nX = rotMat * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
	nY = rotMat * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
	nZ = rotMat * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
}
Cuboid::clRep
Cuboid::getCL() const {
	clRep rep;
	rep.p = cl_float4{cornerP.x, cornerP.y, cornerP.z, 0.0f};
	rep.nX = cl_float4{nX.x, nX.y, nX.z, dims.x};
	rep.nY = cl_float4{nY.x, nY.y, nY.z, dims.y};
	rep.nZ = cl_float4{nZ.x, nZ.y, nZ.z, dims.z};
	return rep;
}
} // namespace brdfEditor::clRenderer
