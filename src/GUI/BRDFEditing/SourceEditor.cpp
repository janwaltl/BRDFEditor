#include "SourceEditor.hpp"

#include <ImGui/imgui.h>

namespace brdfEditor::gui::editor {
namespace {

// ImGui callback to InputTextMultiline which correctly resizes the underlying
// buffer.
int
sourceEditcallback(ImGuiInputTextCallbackData *data) {
	using vec_t = std::vector<char>;
	vec_t *source = reinterpret_cast<vec_t *>(data->UserData);
	if (data->EventFlag == ImGuiInputTextFlags_CallbackResize) {
		bool willBeRelocated = source->capacity() < data->BufTextLen + 1;
		source->resize(data->BufTextLen + 1);
		if (willBeRelocated) {
			data->Buf = source->data();
			data->BufDirty = true;
		}
	}
	return 0;
}

constexpr const int sourceEditFlags = ImGuiInputTextFlags_CallbackResize |
									  ImGuiInputTextFlags_AllowTabInput;
// Draws one ImGuiTab with source editor.
// Must be called inside ImGui::BeginTabBar-EndTabBar.
void
drawTabSourceEditor(BRDFEntry &brdfEntry, EditorParams &params) {
	if (ImGui::BeginTabItem(brdfEntry.name.c_str(), &brdfEntry.opened)) {
		params.selectBRDF(&brdfEntry.name);

		ImGui::InputTextMultiline("SourceEdit", brdfEntry.source.data(),
								  brdfEntry.source.size(),
								  ImGui::GetContentRegionAvail(),
								  sourceEditFlags, sourceEditcallback,
								  &brdfEntry.source);
		ImGui::EndTabItem();
	}
}
} // namespace
SourceEditor::SourceEditor(EditorParams &parameters) : params(&parameters) {}

void
SourceEditor::draw() {
	if (ImGui::BeginTabBar("Open sources",
						   ImGuiTabBarFlags_FittingPolicyScroll |
							   ImGuiTabBarFlags_NoCloseWithMiddleMouseButton)) {
		for (BRDFEntry &brdfEntry : params->getEditedBRDFs()) {
			if (!brdfEntry.opened)
				continue;

			drawTabSourceEditor(brdfEntry, *params);
			// If this the tab has been closed this frame then deselect the
			// current BRDF.
			if (!brdfEntry.opened) {
				// Deselect this BRDF if it was actually selected.
				auto &&maybeSelectedBRDF = params->getSelectedBRDF();
				if (maybeSelectedBRDF && *maybeSelectedBRDF == brdfEntry.name)
					params->selectBRDF(nullptr);
			}
		}
		ImGui::EndTabBar();
	}
}
} // namespace brdfEditor::gui::editor