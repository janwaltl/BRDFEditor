#ifndef BRDFEDITOR_OPENCL_CAMERA_HEADER
#define BRDFEDITOR_OPENCL_CAMERA_HEADER

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include "CLHeaders.hpp"
namespace brdfEditor::clRenderer {
class Camera {
public:
	struct Camera_cl {
		cl_float4 pos;
		cl_float4 dir;
		cl_float4 up;
	};
	using clRep = Camera_cl;

	// Constructs default camera located at the orgin that looks into negative
	// z-direction with y-axis being the up direction.
	Camera();
	// Orients the camera in the world.
	// --upDir: Normalized and must not be paralell with (targetPos-camPos).
	void
	lookAt(glm::vec3 camPos, glm::vec3 targetPos, glm::vec3 upDir);
	// Rotates the camera around its current target
	// --axis: Normalized axis to rotate around.
	// --angle: In radians.
	void
	rotateAround(glm::vec3 axis, float angle);
	clRep
	getCL() const;
	const glm::vec3 &
	getPos() const;
	const glm::vec3 &
	getDir() const;
	const glm::vec3
	getTargetPos() const;
	const glm::vec3
	getUpDir() const;
	glm::mat4
	getView() const;
	glm::mat4
	getInvView() const;

private:
	// Calculates camera direction based on current 'pos' and 'targetPos'.
	glm::vec3
	calcCamDir() const;

	glm::vec3 pos, targetPos, dir, up;
};
} // namespace brdfEditor::clRenderer

#endif
