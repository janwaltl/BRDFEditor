#include "CommonElements.hpp"

#include <filesystem>

#include <ImGui/imgui.h>

namespace brdfEditor::gui {
bool
drawMatParams(const clRenderer::MaterialHandle &matHandle) {
	using namespace clRenderer;
	bool changed = false;
	auto &&[matName, mat] = *matHandle;

	if (mat.params.empty())
		ImGui::Text("No parameters present.");
	for (auto &&[paramName, param] : mat.params) {
		if (param.holdsType<BRDFIntParam>()) {
			auto &p = param.get<BRDFIntParam>();
			changed |= ImGui::SliderInt(paramName.c_str(), &p.value, p.min,
										p.max);
		} else if (param.holdsType<BRDFFloatParam>()) {
			auto &p = param.get<BRDFFloatParam>();
			changed |= ImGui::SliderFloat(paramName.c_str(), &p.value, p.min,
										  p.max);
		} else if (param.holdsType<BRDFFloat2Param>()) {
			auto &p = param.get<BRDFFloat2Param>();
			float tmp[2]{p.value.x, p.value.y};

			if (ImGui::SliderFloat2(paramName.c_str(), tmp, p.min, p.max)) {
				changed = true;
				p.value.x = tmp[0];
				p.value.y = tmp[1];
			}
		} else if (param.holdsType<BRDFFloat3Param>()) {
			auto &p = param.get<BRDFFloat3Param>();
			float tmp[3]{p.value.x, p.value.y, p.value.z};
			bool res = p.isColor ? ImGui::ColorEdit3(paramName.c_str(), tmp)
								 : ImGui::SliderFloat3(paramName.c_str(), tmp,
													   p.min, p.max);
			if (res) {
				changed = true;
				p.value.x = tmp[0];
				p.value.y = tmp[1];
				p.value.z = tmp[2];
			}
		} else if (param.holdsType<BRDFFloat4Param>()) {
			auto &p = param.get<BRDFFloat4Param>();
			float tmp[4]{p.value.x, p.value.y, p.value.z, p.value.w};

			if (ImGui::SliderFloat4(paramName.c_str(), tmp, p.min, p.max)) {
				changed = true;
				p.value.x = tmp[0];
				p.value.y = tmp[1];
				p.value.z = tmp[2];
				p.value.w = tmp[3];
			}
		} else
			assert("Forgot to handle some BRDFParam type.");
	}
	return changed;
}
namespace {
template <typename Scene, typename MatHandle>
void
drawMaterialsComboImpl(Scene &scene,
					   std::optional<MatHandle> &selectedMaterial) {

	const char *selectedMatStr = selectedMaterial
									 ? (*selectedMaterial)->first.c_str()
									 : "Select a material";
	if (ImGui::BeginCombo("##Materials", selectedMatStr)) {
		for (auto &&matHandle : scene.getMaterials()) {
			auto &&[matName, mat] = *matHandle;
			bool selected = matHandle == selectedMaterial;
			if (ImGui::Selectable(matName.c_str(), selected))
				selectedMaterial = matHandle;
		}
		ImGui::EndCombo();
	}
}
} // namespace
void
drawMaterialsCombo(
	const clRenderer::Scene &scene,
	std::optional<clRenderer::MaterialCHandle> &selectedMaterial) {
	return drawMaterialsComboImpl(scene, selectedMaterial);
}

void
drawMaterialsCombo(
	clRenderer::Scene &scene,
	std::optional<clRenderer::MaterialHandle> &selectedMaterial) {
	return drawMaterialsComboImpl(scene, selectedMaterial);
}
void
drawInputFilenameCombo(const std::string &directory,
					   std::optional<std::string> &selectedFilename,
					   const std::string &defaultText /*= "Select a file"*/) {
	namespace fs = std::filesystem;

	
	const auto& preview = selectedFilename.value_or(defaultText);

	if (ImGui::BeginCombo("##Scene filename", preview.c_str())) {
		for (auto &&entry : fs::directory_iterator(directory)) {
			if (!entry.is_regular_file())
				continue;
			auto &&filename = entry.path().string();
			if (ImGui::Selectable(filename.c_str()))
				selectedFilename = filename;
		}
		ImGui::EndCombo();
	}
}
void drawTitle(const char * str)
{
	if (ImGui::BeginMenuBar()) {
		ImGui::Text(str);
		ImGui::EndMenuBar();
	}
}
} // namespace brdfEditor::gui