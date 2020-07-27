#ifndef BRDFEDITOR_GUI_EDITING_EDITOR_HEADER
#define BRDFEDITOR_GUI_EDITING_EDITOR_HEADER

#include <src/GUI/TwoPanelLayout.hpp>
#include <src/GUI/SceneEvents.hpp>
#include "BRDFMatList.hpp"
#include "BRDFMatParams.hpp"
#include "EditorParams.hpp"
#include "SourceEditor.hpp"

namespace brdfEditor {
class Program;
namespace gui::editor {

// Responsible for drawing the UI that allows editing of BRDFs. UI offers basic
// source editor, kernel compilator and material manager. Also using
// TwoPanelLayout gives access to the console.
class Editor : public TwoPanelLayout {
public:
	Editor(Program &program, SceneEvents& sceneEvents, Console &console, GUIParams &parameters);

private:
	virtual void
	leftPanelContent() override final;
	virtual void
	mainPanelContent() override final;

	Program *program;
	SceneEvents* sceneEvents;
	EditorParams eParams;
	BRDFMatList brdfMatList;
	BRDFMatParams brdfMatParams;
	SourceEditor sourceEditor;
};
} // namespace gui::editor
} // namespace brdfEditor
#endif // !BRDFEDITOR_GUI_EDITING_EDITOR_HEADER
