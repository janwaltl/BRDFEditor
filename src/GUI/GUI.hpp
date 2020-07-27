#ifndef BRDFEDITOR_GUI_GUI_HEADER
#define BRDFEDITOR_GUI_GUI_HEADER

#include <cstddef>
#include <memory>

#include <ImGui/imgui.h>

#include "BRDFEditing/Editor.hpp"
#include "Graphs/Graphs.hpp"
#include "GUIParams.hpp"
#include "SceneEvents.hpp"
#include "SceneRendering/SceneRendering.hpp"

namespace brdfEditor {
class Program;
}
namespace brdfEditor::gui {
class SceneRendering;
class BRDFTextEditor;

// Class that encapsulates all UI rendering.
// Needs valid ImGui context
class GUI {
public:
	// --w,h: Width and height of the screen.
	GUI(float w, float h, Program &p);
	~GUI();
	// Draws the GUI
	// --w,h: Current size of the screen.
	void
	draw(float w, float h);

	Console& getConsole();
private:
	enum class Window { renderedScene, brdfTextEditor, graphs };

	void
	drawMenuBar();
	// Updates gParams variable based on the current screen resolution.
	void
	updateGUIParams(float w, float h);
	Program *program;
	GUIParams gParams;

	Console console;
	SceneEvents sceneEvents;
	sceRen::SceneRendering sceneRendering;
	editor::Editor brdfEditor;
	graphs::Graphs graphs;
	// GUI style used by the application. Saved here to allow lossless, repeated
	// scaling.
	ImGuiStyle backupStyle;
	Window selectedWindow;
	// Initializes GUI parameters on the first draw() because updateGUIParams
	// must be called from inside begin,renderFrame of WindowManager.
	// - ImGui updates some variables only on frame-to-frame basis.
	bool firstDraw;
};
} // namespace brdfEditor::gui

#endif