#ifndef BRDFEDITOR_GUI_SCENE_RENDERING_SCENE_TOOLS_HEADER
#define BRDFEDITOR_GUI_SCENE_RENDERING_SCENE_TOOLS_HEADER

namespace brdfEditor::clRenderer {
class Scene;
class Renderer;
} // namespace brdfEditor::clRenderer
namespace brdfEditor::gui::sceRen {
// Draws a collapsing header that contains UI elements to access the scene and
// change it. Allows changing of light properties.
class SceneTools {
public:
	SceneTools(clRenderer::Scene &scene);
	// Assumes that the passed renderer contains the same scene as the one
	// passed  in the constructor. If the scene has been somehow changed then it
	// will send a RecompilationFlags::SceneObjects flag to the renderer.
	void
	draw(clRenderer::Renderer &renderer);

private:
	void drawSetEnvMapPopup(clRenderer::Renderer &renderer,clRenderer::Scene& scene);
	bool setEnvMapPopupOpen;
	clRenderer::Scene *scene;
};
} // namespace brdfEditor::gui::sceRen
#endif // !BRDFEDITOR_GUI_SCENE_RENDERING_SCENE_TOOLS_HEADER
