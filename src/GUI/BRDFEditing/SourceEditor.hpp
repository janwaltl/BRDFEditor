#ifndef BRDFEDITOR_GUI_EDITING_SOURCE_EDITOR_HEADER
#define BRDFEDITOR_GUI_EDITING_SOURCE_EDITOR_HEADER

#include "EditorParams.hpp"

namespace brdfEditor::gui::editor {
// A GUI element that draws a BRDF source editor. Which is a collection of
// opened tabs with individual BRDFs and very simple text input field for
// editing.
class SourceEditor {
public:
	SourceEditor(EditorParams &parameters);
	// Expects to be called in the context of an opened ImGui Window. Draws a
	// tab bar and fills rest of the window with an input text field.
	void
	draw();

private:
	EditorParams *params;
};
} // namespace brdfEditor::gui::editor
#endif // !BRDFEDITOR_GUI_EDITING_SOURCE_EDITOR_HEADER
