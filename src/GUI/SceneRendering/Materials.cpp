#include "Materials.hpp"

#include <ImGui/imgui.h>

#include <src/GUI/CommonElements.hpp>
#include <src/OpenCLRenderer/Scene.hpp>
#include <src/OpenCLRenderer/Renderer/Renderer.hpp>

namespace brdfEditor::gui::sceRen {
Materials::Materials(clRenderer::Scene &scene) : scene(&scene) {}

void
Materials::draw(clRenderer::Renderer &renderer) {
	using namespace clRenderer;

	if (!ImGui::CollapsingHeader("Scene Materials",
								 ImGuiTreeNodeFlags_DefaultOpen))
		return;
	if (!ImGui::BeginChild("Scene Materials Child", ImVec2(0, 300), true))
		return;

	bool changed = false;
	std::size_t ID = 0; // Unique ID for UI elements
	for (auto &&matHandle : scene->getMaterials()) {
		ImGui::PushID(ID++);
		auto &&[matName, mat] = *matHandle;
		if (ImGui::CollapsingHeader(matName.c_str(),
									ImGuiTreeNodeFlags_DefaultOpen))
			changed |= drawMatParams(matHandle);
		ImGui::PopID();
	}
	if (changed)
		renderer.requestRecompilation(
			Renderer::RecompilationFlags::MaterialsParameters);

	ImGui::EndChild();
}

} // namespace brdfEditor::gui::sceRen
