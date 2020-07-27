#ifndef BRDFEDITOR_GUI_EDITING_BRDF_MAT_PARAMS_HEADER
#define BRDFEDITOR_GUI_EDITING_BRDF_MAT_PARAMS_HEADER

#include "EditorParams.hpp"

namespace brdfEditor {

namespace clRenderer {
class Scene;
}
namespace gui::editor {

// Draws two collapsing headers - one showing parameters of currently selected
// BRDF, another does the same for the selected material.
class BRDFMatParams {
public:
	BRDFMatParams(clRenderer::Scene &scene, EditorParams &params);

	void
	draw();

private:
	// First collapsing header with summaries of selected BRDF parameters.
	void
	drawBRDFProperties();
	// Second collapsing header with controls for selected material parameters.
	void
	drawMatProperties();
	EditorParams *params;
	clRenderer::Scene *scene;
};
} // namespace gui::editor
} // namespace brdfEditor

#endif // !BRDFEDITOR_GUI_EDITING_BRDF_MAT_PARAMS_HEADER
