#include "BRDFMatParams.hpp"

#include <ImGui/imgui.h>

#include <src/OpenCLRenderer/Scene.hpp>
#include <src/GUI/CommonElements.hpp>

namespace brdfEditor::gui::editor {
namespace {
// Draws text summary of passed parameter. Shows its type, default, min and max
// values.
void
drawParam(const clRenderer::BRDFParam &param) {
	using namespace clRenderer;

	param.visit([](auto &&param) {
		using ParamT = std::decay_t<decltype(param)>;

		if constexpr (std::is_same_v<ParamT, BRDFIntParam>) {
			ImGui::BulletText("type: int");
			ImGui::BulletText("default: %d", param.value);
			ImGui::BulletText("min: %d", param.min);
			ImGui::BulletText("max: %d", param.max);
		} else if constexpr (std::is_same_v<ParamT, BRDFFloatParam>) {
			ImGui::BulletText("type: float");
			ImGui::BulletText("default: %f", param.value);
			ImGui::BulletText("min: %f", param.min);
			ImGui::BulletText("max: %f", param.max);
		} else if constexpr (std::is_same_v<ParamT, BRDFFloat2Param>) {
			ImGui::BulletText("type: float2");
			ImGui::BulletText("default: %f %f", param.value.x, param.value.y);
			ImGui::BulletText("min: %f", param.min);
			ImGui::BulletText("max: %f", param.max);
		} else if constexpr (std::is_same_v<ParamT, BRDFFloat3Param>) {
			ImGui::BulletText("type: %s", param.isColor ? "color" : "float3");
			ImGui::BulletText("default: %f %f", param.value.x, param.value.y);
			ImGui::BulletText("min: %f", param.min);
			ImGui::BulletText("max: %f", param.max);
		} else if constexpr (std::is_same_v<ParamT, BRDFFloat4Param>) {
			ImGui::BulletText("type: float4");
			ImGui::BulletText("default: %f %f", param.value.x, param.value.y);
			ImGui::BulletText("min: %f", param.min);
			ImGui::BulletText("max: %f", param.max);
		} else
			static_assert(false, "Does not cover all variant types.");
	});
}
} // namespace

BRDFMatParams::BRDFMatParams(clRenderer::Scene &scene, EditorParams &params) :
	scene(&scene),
	params(&params) {}

void
BRDFMatParams::draw() {
	ImGui::BeginGroup();
	if (ImGui::CollapsingHeader("BRDF properties",
								ImGuiTreeNodeFlags_DefaultOpen))
		drawBRDFProperties();

	ImGui::EndGroup();

	ImGui::BeginGroup();
	if (ImGui::CollapsingHeader("Material properties",
								ImGuiTreeNodeFlags_DefaultOpen))
		drawMatProperties();

	ImGui::EndGroup();
}

void
BRDFMatParams::drawBRDFProperties() {
	auto &&maybeSelectedBRDF = params->getSelectedBRDF();

	if (!maybeSelectedBRDF) {
		ImGui::TextWrapped(
			"No BRDF selected. You can select one by clicking on "
			"\"Edit\" and then open the corresponding tab.");
	} else {
		const auto &brdfName = (*maybeSelectedBRDF);
		auto &&maybeBrdfHandle = scene->getBRDF(brdfName);
		assert(maybeBrdfHandle);
		auto &&[brdfName2, brdf] = **maybeBrdfHandle;
		drawTitle("Name");
		ImGui::Text("Name: %s", brdfName.c_str());
		const char *customSampling = brdf.hasCustomSampling() ? "Yes" : "No";
		std::size_t numParameters = brdf.parameters.size();
		ImGui::Text("Custom sampling: %s", customSampling);
		ImGui::Text("Detected parameters: %zu", numParameters);
		auto paramHeight = ImGui::GetItemsLineHeightWithSpacing() * 5;
		for (auto &&[paramName, param] : brdf.parameters) {
			if (ImGui::BeginChild(paramName.c_str(), ImVec2(0, paramHeight),
								  true, ImGuiWindowFlags_MenuBar)) {
				drawTitle(paramName.c_str());
				drawParam(param);
			}
			ImGui::EndChild();
		}
	}
}

void
BRDFMatParams::drawMatProperties() {
	auto &&maybeSelectedMat = params->getSelectedMaterial();
	if (!maybeSelectedMat) {
		ImGui::TextWrapped(
			"No material selected. You can select one by clicking on "
			"\"Edit\" next to it.");
	} else {
		ImGui::Text("Name: %s", maybeSelectedMat->c_str());
		auto &&maybeMatHandle = scene->getMaterial(*maybeSelectedMat);
		assert(maybeMatHandle);
		drawMatParams(*maybeMatHandle);
	}
}

} // namespace brdfEditor::gui::editor
