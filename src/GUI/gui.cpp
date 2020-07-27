#include "GUI.hpp"

#include <ImGui/imgui.h>

#include <src/Core/Program.hpp>
#include "GUIParams.hpp"

namespace brdfEditor::gui {

GUI::GUI(float w, float h, Program &p) :
	program(&p),
	gParams(),
	sceneRendering(p, sceneEvents, console, gParams),
	brdfEditor(p, sceneEvents, console, gParams),
	graphs(p.getRenderer(), sceneEvents, p.getScene(), gParams),
	selectedWindow(Window::renderedScene),
	backupStyle(ImGui::GetStyle()),
	firstDraw(true) {}

GUI::~GUI() {}

void
GUI::draw(float w, float h) {
	updateGUIParams(w, h);
	drawMenuBar();
	switch (selectedWindow) {
	case brdfEditor::gui::GUI::Window::renderedScene:
		sceneRendering.draw();
		break;
	case brdfEditor::gui::GUI::Window::brdfTextEditor:
		brdfEditor.draw();
		break;
	case brdfEditor::gui::GUI::Window::graphs:
		graphs.draw();
		break;
	default:
		// Forgot to add an enum
		assert(0);
		break;
	}
}
Console & GUI::getConsole()
{
	return console;
}
void
GUI::drawMenuBar() {
	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::Button("Scene Rendering"))
			selectedWindow = Window::renderedScene;
		ImGui::SameLine();
		if (ImGui::Button("BRDF Editor")) {
			program->pauseRendering();
			selectedWindow = Window::brdfTextEditor;
		}
		ImGui::SameLine();
		if (ImGui::Button("Graphs")) {
			program->pauseRendering();
			selectedWindow = Window::graphs;
		}
		ImGui::SameLine();
		ImGui::EndMainMenuBar();
	}
}

void
GUI::updateGUIParams(float w, float h) {

	const glm::vec2 currRes{w, h};
	constexpr const glm::vec2 baseRes = GUIParams::baseResolution;
	if (firstDraw || gParams.resolution() != currRes) // If nothing changed
	{
		firstDraw = false;

		gParams.res = currRes;
		gParams.scale = gParams.res / baseRes;

		ImGui::GetStyle() = backupStyle;
		// TEMP Only uses X to scale.
		// Correct way would be to multiply ImVec2 style parameters by both.
		// - Will work if the aspect ratio is constant.
		ImGui::GetStyle().ScaleAllSizes(gParams.scale.x);
	}

	ImGui::GetIO().FontGlobalScale = std::max(1.0f, gParams.scale.y);

	// Leave space for the main menu bar
	// No need to scale this, because it should be scaled by style&font
	// IMPROVE Currently has one-frame lag and so must be updated each frame.
	//	-Cause by FontGlobalScale not automatically updating font sizes
	gParams.canvas.position = {0.0f, ImGui::GetFrameHeight()};
	// Rest of the window is used as the canvas.
	gParams.canvas.size = baseRes * gParams.scale - gParams.canvas.position;
}
} // namespace brdfEditor::gui
