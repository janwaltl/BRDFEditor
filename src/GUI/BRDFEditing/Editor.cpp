#include "Editor.hpp"

#include <src/Core/Program.hpp>

namespace brdfEditor::gui::editor {

Editor::Editor(Program &program, SceneEvents &sceneEvents, Console &console,
			   GUIParams &parameters) :
	TwoPanelLayout(console, parameters,"Tools","Editor"),
	program(&program),
	sceneEvents(&sceneEvents),
	eParams(program.getScene(),sceneEvents, console),
	brdfMatList(program.getRenderer(), eParams),
	brdfMatParams(program.getScene(), eParams),
	sourceEditor(eParams) {
}

void
Editor::leftPanelContent() {
	brdfMatList.draw();
	brdfMatParams.draw();
}

void
Editor::mainPanelContent() {
	sourceEditor.draw();
}
} // namespace brdfEditor::gui::editor