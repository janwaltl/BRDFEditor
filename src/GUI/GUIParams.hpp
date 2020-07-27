#ifndef BRDFEDITOR_GUI_GUIPARAMS_HEADER
#define BRDFEDITOR_GUI_GUIPARAMS_HEADER

#include <glm/vec2.hpp>
#include <ImGui/imgui.h>

namespace brdfEditor::gui {
class GUI;

// Various parameters of the GUI.
// Should provide constants for other parts of the GUI
// to achieve consistent, resolution-free interface.
class GUIParams {
public:
	constexpr const static glm::vec2 baseResolution{1920.0, 1080.0};

	// Screen resolution in pixels.
	const glm::vec2 &
	resolution() const;
	// Resolution relative to the baseResolution.
	const glm::vec2 &
	resolutionScale() const;
	// Pixel coordinates of the canvas of the program.
	// Its top left corner. The canvas is the GUI area available for drawing.
	const glm::vec2 &
	canvasPos() const;
	// Size of the drawable canvas
	const glm::vec2 &
	canvasSize() const;

private:
	friend class GUI;
	// Screen resolution in pixels.
	glm::vec2 res;
	// Scale relative to fullHD resolution
	glm::vec2 scale;
	// Pixel coordinates of the available drawable GUI area.
	struct {
		glm::vec2 size, position;
	} canvas;
};
} // namespace brdfEditor::gui
#endif
