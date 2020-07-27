#ifndef BRDFEDITOR_GUI_TWO_PANEL_LAYOUT_HEADER
#define BRDFEDITOR_GUI_TWO_PANEL_LAYOUT_HEADER

#include <string>

#include <glm/vec2.hpp>

#include "Console.hpp"
#include "GUIParams.hpp"

namespace brdfEditor::gui {

// Collection of parameters defining the TwoPanelLayout abstract class.
class TwoPanelParams {
public:
	Console &
	getConsole() const;
	bool
	isConsoleOpen() const;
	void
	switchConsole(bool open);
	// Returns: number in [0,1] representing the relative size of the left
	// panel. 0.5 means that the left and main panels have the same width.
	float
	dividerPos() const;
	// Pixel coordinates of the top-left corner of the left panel.
	const glm::vec2 &
	leftCanvasPos() const;
	// Size of the drawable canvas in pixels
	const glm::vec2 &
	leftCanvasSize() const;
	// Pixel coordinates of the top-left corner of the main panel.
	const glm::vec2 &
	mainCanvasPos() const;
	// Size of the drawable canvas
	const glm::vec2 &
	mainCanvasSize() const;
	const std::string &
	leftTitle() const;
	std::string &
	leftTitle();
	const std::string &
	mainTitle() const;
	std::string &
	mainTitle();

private:
	friend class TwoPanelLayout;
	// Pixel coordinates of the top-left corner of the console panel.
	const glm::vec2 &
	consoleCanvasPos() const;
	// Size of the drawable canvas
	const glm::vec2 &
	consoleCanvasSize() const;

	Console *console;
	bool consoleOpened;
	float divider;
	// Pixel coordinates of the available drawable GUI area.
	struct {
		glm::vec2 size, position;
		std::string title;
	} leftCanvas, mainCanvas;
	// Pixel coordinates of the available drawable GUI area.
	glm::vec2 consolePos, consoleSize;
};

// Abstract class that draws a generic UI with two main panels and a console.
class TwoPanelLayout {
public:
	TwoPanelLayout(Console &console, GUIParams &parameters);
	TwoPanelLayout(Console &console, GUIParams &parameters,std::string leftTitle,std::string rightTitle);
	virtual ~TwoPanelLayout() = default;
	// Draws the UI.
	void
	draw();

protected:
	const TwoPanelParams &
	parameters();

private:
	// Updates panelParams each frame based on the values obtained from
	// guiParams. Also clears Console new input flag
	void
	updateParameters();
	// Sub method of updateParameters method, also reflects console status.
	void
	updateMainPanelParameters();
	// Draws the content of the console passed in the constructor and stored in
	// panelParams.
	void
	drawConsole();
	virtual void
	leftPanelContent() = 0;
	virtual void
	mainPanelContent() = 0;

	const GUIParams *guiParams;
	TwoPanelParams panelParams;
};
} // namespace brdfEditor::gui
#endif // !BRDFEDITOR_GUI_TWO_PANEL_LAYOUT_HEADER
