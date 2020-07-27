#ifndef BRDFEDITOR_GUI_SCENE_RENDERING_MATERIALS_HEADER
#define BRDFEDITOR_GUI_SCENE_RENDERING_MATERIALS_HEADER

namespace brdfEditor::clRenderer {
class Scene;
class Renderer;
} // namespace brdfEditor::clRenderer
namespace brdfEditor::gui::sceRen {

// Draws a collapsing header that contains all materials from the scene with
// their parameters. UI allows tweaking the parameters which sends recompilation
// request to the passed renderer.
class Materials {
public:
	Materials(clRenderer::Scene &scene);
	// If any material'parameter is changed the
	// Renderer::RecompilationFlags::MaterialsParameters flag is sent to the
	// passed renderer.
	// --renderer: Assumes that it renders the same scene as the one received in
	//			   the constructor.
	void
	draw(clRenderer::Renderer &renderer);

private:
	clRenderer::Scene *scene;
};
} // namespace brdfEditor::gui::sceRen
#endif // !BRDFEDITOR_GUI_SCENE_RENDERING_MATERIALS_HEADER
