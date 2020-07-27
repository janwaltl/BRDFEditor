#include "GUIParams.hpp"

namespace brdfEditor::gui {
const glm::vec2 &
GUIParams::resolution() const {
	return res;
}
const glm::vec2 &
GUIParams::resolutionScale() const {
	return scale;
}
const glm::vec2 &
GUIParams::canvasPos() const {
	return canvas.position;
}
const glm::vec2 &
GUIParams::canvasSize() const {
	return canvas.size;
}
} // namespace brdfEditor::gui
