#ifndef BRDFEDITOR_GUI_EDITING_EDITOR_BRDF_MAT_LIST_HEADER
#define BRDFEDITOR_GUI_EDITING_EDITOR_BRDF_MAT_LIST_HEADER

#include <optional>
#include <string>

#include "EditorParams.hpp"

namespace brdfEditor {

namespace clRenderer {
class Renderer;
}
namespace gui::editor {

// An UI element that draws a list of BRDFs and materials associated with them.
// Contains tools to create a new BRDF or materials as well as remove them.
class BRDFMatList {
public:
	BRDFMatList(clRenderer::Renderer &renderer, EditorParams &params);
	void
	draw();

private:
	// Draws list of BRDF present in the scene.
	// -uniqueID: Is used for differentiating between ImGui buttons with the
	// same text.
	void
	drawBRDFs(clRenderer::Scene &scene, int &uniqueID);
	// Draws list of materials associated with given BRDF
	// -uniqueID: Is used for differentiating between ImGui buttons with the
	// same text.
	void
	drawMaterials(clRenderer::BRDF &brdf, int &uniqueID,
				  const std::string &brdfName);

	void
	drawPopups();
	void
	addMaterialPopup();
	void
	newBRDFPopup();
	EditorParams *params;
	clRenderer::Renderer *renderer;
	struct {
		bool open;
		bool justOpened;
		bool nameExists;
		constexpr const static std::size_t matNameMaxLen = 256;
		char matName[matNameMaxLen];
		std::string brdfName;
	} addMatPopupData;
	// To which BRDf add the new material.
	std::string addMaterialBRDFName;
	struct {
		bool open;
		bool justOpened;
		bool nameExists;
		constexpr const static std::size_t nameMaxLen = 256;
		char name[nameMaxLen];
		bool pasteFromClipboardCheck;
		std::optional<std::string> selectedBRDFFile;
	} newBRDFPopupData;
};
} // namespace gui::editor
} // namespace brdfEditor
#endif // !BRDFEDITOR_GUI_EDITING_EDITOR_BRDF_MAT_LIST_HEADER
