#ifndef BRDFEDITOR_GUI_COMMON_ELEMENTS_HEADER
#define BRDFEDITOR_GUI_COMMON_ELEMENTS_HEADER

#include <optional>
#include <string>

#include <src/OpenCLRenderer/Materials/Material.hpp>
#include <src/OpenCLRenderer/Scene.hpp>

#include "GUIParams.hpp"
#include <external/ImGui/imgui.h>

namespace brdfEditor::gui {

// Draws an ImGui::Slider variant for each material's parameter based on its
// type. Allows users to change them. Is not embedded inside any window
// Returns: True if the user changed any parameter.
bool
drawMatParams(const clRenderer::MaterialHandle &matHandle);

// Draws a combo box listing all scene's materials and allows to choose one.
// -- selectedMaterial: Which material should be marked as selected in the combo
// box. If a user chooses different material then the variable stores that
// result. The function never empties this variable.
void
drawMaterialsCombo(
	const clRenderer::Scene &scene,
	std::optional<clRenderer::MaterialCHandle> &selectedMaterial);
// See const scene overload.
void
drawMaterialsCombo(clRenderer::Scene &scene,
				   std::optional<clRenderer::MaterialHandle> &selectedMaterial);

// Draws unnamed ImGui::Combo that lists all files present in 'directory' and
// allows to select one.
// - directory: Relative or absolute, does not end with '/'.
// - selectedFilename: Will contain selected filename and is used as selected
// item in the drawn combo. Returns: Through `selectedFilename` argument.
void
drawInputFilenameCombo(const std::string &directory,
					   std::optional<std::string> &selectedFilename,
					   const std::string &defaultText = "Select a file");

constexpr const ImGuiWindowFlags menuNoTitle = ImGuiWindowFlags_MenuBar |
											   ImGuiWindowFlags_NoScrollbar |
											   ImGuiWindowFlags_NoTitleBar;

// Draws the title in the menu bar of a window.
void
drawTitle(const char *str);
// Draws custom content in the menu bar of a window.
// Offers more flexibility than one string.
// --contentFnc: void(void) callable.
template <typename ContentFnc>
void
drawTitle(ContentFnc &&contentFnc) {
	if (ImGui::BeginMenuBar()) {
		contentFnc();
		ImGui::EndMenuBar();
	}
}
} // namespace brdfEditor::gui
#endif // !BRDFEDITOR_GUI_COMMON_ELEMENTS_HEADER
