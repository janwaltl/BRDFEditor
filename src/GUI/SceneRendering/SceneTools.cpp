#include "SceneTools.hpp"

#include <filesystem>

#include <ImGui/imgui.h>

#include <src/GUI/CommonElements.hpp>
#include <src/OpenCLRenderer/Scene.hpp>
#include <src/OpenCLRenderer/Renderer/Renderer.hpp>

namespace brdfEditor::gui::sceRen {
SceneTools::SceneTools(clRenderer::Scene &scene) : scene(&scene) {}

namespace {
constexpr const char *envMapPopupStr = "Set new environment map";
// Draws a list of passed objects, each with a material selector.
// --O: must derive from clRenderer::Object class
template <typename O>
bool
drawMatSelectorForObjects(clRenderer::Scene &scene, std::vector<O *> &objects) {
	static_assert(std::is_base_of_v<clRenderer::Object, O>);
	ImGui::Columns(2);
	bool objChanged = false;
	for (auto &&object : objects) {
		ImGui::Text(object->getName().c_str());
		ImGui::SameLine();
		std::optional<clRenderer::MaterialCHandle> matHandle =
			object->getMatHandle();
		ImGui::PushID(object);
		drawMaterialsCombo(scene, matHandle);
		ImGui::PopID();
		if (matHandle != object->getMatHandle()) {
			object->setMatHandle(*matHandle);
			objChanged = true;
		}
		ImGui::NextColumn();
	}
	ImGui::Columns(1);
	return objChanged;
}
template <typename T, typename Fnc>
bool
drawLights(std::vector<T> &lights, const char *childName, std::size_t lines,
		   Fnc &&fnc) {
	bool objChanged = false;
	int i = 0;
	for (auto &light : lights) {
		ImGui::PushID(++i);
		// Heighten the line to keep small empty borders
		float lineHeight = 1.1f * ImGui::GetItemsLineHeightWithSpacing();
		if (ImGui::BeginChild(childName, ImVec2(0, lines * lineHeight), false,
							  ImGuiWindowFlags_NoScrollWithMouse |
								  ImGuiWindowFlags_MenuBar |
								  ImGuiWindowFlags_AlwaysUseWindowPadding)) {
			objChanged |= fnc(light);
		}
		ImGui::EndChild();
		ImGui::PopID();
	}
	return objChanged;
}
bool
drawLights(std::vector<clRenderer::AreaLight> &areaLights) {
	return drawLights(
		areaLights, "Area Light", 4, [](clRenderer::AreaLight &light) {
			drawTitle(light.getName().c_str());
			bool objChanged = false;
			objChanged |= ImGui::ColorEdit3("Color", &light.col.x);
			objChanged |= ImGui::DragFloat("Intensity", &light.intensity, 0.1f,
										   0.0f, 1e10f);
			objChanged |= ImGui::DragFloat("HalfSize", &light.halfSize, 0.1f,
										   0.0f, 1e10f);
			return objChanged;
		});
}
bool
drawLights(std::vector<clRenderer::SphereLight> &sphereLights) {
	return drawLights(
		sphereLights, "Sphere Light", 4, [](clRenderer::SphereLight &light) {
			drawTitle(light.getName().c_str());
			bool objChanged = false;
			objChanged |= ImGui::ColorEdit3("Color", &light.color.x);
			objChanged |= ImGui::DragFloat("Intensity", &light.intensity, 0.1f,
										   0.0f, 1e10f);
			objChanged |=
				ImGui::DragFloat("Radius", &light.radius, 0.1f, 0.0f, 1e10f);
			return objChanged;
		});
}
bool
drawLights(std::vector<clRenderer::PointLight> &pointLights) {
	return drawLights(
		pointLights, "Point Light", 3, [](clRenderer::PointLight &light) {
			drawTitle(light.getName().c_str());
			bool objChanged = false;
			objChanged |= ImGui::ColorEdit3("Color", &light.col.x);
			objChanged |= ImGui::DragFloat("Intensity", &light.intensity, 0.1f,
										   0.0f, 1e10f);
			return objChanged;
		});
}

} // namespace
void
SceneTools::draw(clRenderer::Renderer &renderer) {
	if (setEnvMapPopupOpen)
		drawSetEnvMapPopup(renderer, *scene);

	bool objChanged = false;
	ImGui::BeginGroup();
	if (ImGui::CollapsingHeader("Scene Objects",
								ImGuiTreeNodeFlags_DefaultOpen))
		objChanged |= drawMatSelectorForObjects(*scene, scene->getObjects());

	ImGui::EndGroup();

	ImGui::BeginGroup();
	if (ImGui::CollapsingHeader("Scene Lights",
								ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::Text("Environment map:");
		ImGui::SameLine();
		if (ImGui::Button("Change")) {
			ImGui::OpenPopup(envMapPopupStr);
			setEnvMapPopupOpen = true;
		}
		ImGui::SameLine();
		ImGui::PushItemWidth(50);
		objChanged |= ImGui::DragFloat(
			"Intensity", &scene->getEnvMap().intensity, 0.1f, 0.0f, 1e5f);
		ImGui::PopItemWidth();
		objChanged |= drawLights(scene->getAreaLights());
		objChanged |= drawLights(scene->getSphereLights());
		objChanged |= drawLights(scene->getPointLights());
	}
	ImGui::EndGroup();

	if (objChanged)
		renderer.requestRecompilation(
			clRenderer::Renderer::RecompilationFlags::SceneObjects |
			clRenderer::Renderer::RecompilationFlags::MaterialsParameters);
}

void
SceneTools::drawSetEnvMapPopup(clRenderer::Renderer &renderer,
							   clRenderer::Scene &scene) {
	namespace fs = std::filesystem;
	if (ImGui::BeginPopupModal(envMapPopupStr, &setEnvMapPopupOpen,
							   ImGuiWindowFlags_AlwaysAutoResize)) {
		static std::optional<std::string> newFilename;

		bool close = false;
		const char *preview =
			newFilename ? newFilename->c_str() : "Select a filename";
		if (ImGui::BeginCombo("##Env filename", preview)) {
			for (auto &&entry : fs::directory_iterator("./HDR")) {
				if (!entry.is_regular_file())
					continue;
				auto &&filename = entry.path().string();
				if (ImGui::Selectable(filename.c_str()))
					newFilename = filename;
			}
			ImGui::EndCombo();
		}
		ImGui::Text(
			"Note that the loading may take a while because the map must be \n"
			"processed by the square histogram algorithm.");
		if (newFilename && ImGui::Button("Set")) {
			try {
				scene.setHDREnvironmentMap(*newFilename);
				renderer.requestRecompilation(
					clRenderer::Renderer::RecompilationFlags::SceneObjects);
				close = true;
			} catch (const Exception &e) {
				// TODO Console
			}
		}
		if (newFilename)
			ImGui::SameLine();
		if (ImGui::Button("Cancel"))
			close = true;

		if (close) {
			ImGui::CloseCurrentPopup();
			setEnvMapPopupOpen = false;
		}
		ImGui::EndPopup();
	}
}

} // namespace brdfEditor::gui::sceRen