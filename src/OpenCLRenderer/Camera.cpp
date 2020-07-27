#include "Camera.hpp"

#include <glm/geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace brdfEditor::clRenderer {
Camera::Camera() :
	pos(0.0f, 0.0f, 0.0f),
	targetPos(0.0f, 0.0f, -1.0f),
	dir(calcCamDir()),
	up(0.0f, 1.0f, 0.0f) {}

void
Camera::lookAt(glm::vec3 camPos, glm::vec3 targetPos, glm::vec3 upDir) {
	pos = camPos;
	this->targetPos = targetPos;
	dir = calcCamDir();
	up = upDir;
	up = glm::normalize(up);

}

void
Camera::rotateAround(glm::vec3 axis, float angle) {
	auto rotM = glm::rotate(glm::mat4(1.0f), angle, axis);
	// Rotates the position around the target
	pos = glm::vec3(rotM * glm::vec4(pos - targetPos, 0.0f)) + targetPos;
	dir = calcCamDir();
	up = glm::vec3(rotM * glm::vec4(up, 0.0f));
	up = glm::normalize(up);

}
Camera::clRep
Camera::getCL() const {
	return clRep{cl_float4{pos.x, pos.y, pos.z, 0.0f},
				 cl_float4{dir.x, dir.y, dir.z, 0.0f},
				 cl_float4{up.x, up.y, up.z, 0.0f}};
}
const glm::vec3 &
Camera::getPos() const {
	return pos;
}
const glm::vec3 &
Camera::getDir() const {
	return dir;
}
const glm::vec3
Camera::getTargetPos() const {
	return targetPos;
}
const glm::vec3
Camera::getUpDir() const {
	return up;
}
glm::mat4
Camera::getView() const {
	return glm::lookAt(pos, targetPos, up);
}
glm::mat4
Camera::getInvView() const {
	// LookAt is orthogonal matrix.
	return glm::transpose(getView());
}
glm::vec3
Camera::calcCamDir() const {
	return glm::normalize(targetPos - pos);
}
} // namespace brdfEditor::clRenderer
