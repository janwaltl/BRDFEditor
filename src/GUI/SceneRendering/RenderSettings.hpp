#ifndef BRDFEDITOR_GUI_SCENE_RENDERING_RENDER_SETTINGS_HEADER
#define BRDFEDITOR_GUI_SCENE_RENDERING_RENDER_SETTINGS_HEADER

#include <src/GUI/Console.hpp>

namespace brdfEditor {
class Program;
namespace clRenderer {
class Scene;
class Renderer;
struct PlatformInfo;
} // namespace clRenderer
namespace gui {
class SceneEvents;
}
namespace gui::sceRen {
// Draws a collpasing header contaning UI for render settings. Allows selecting
// the OpenCL platform and the device, image resolution, the scene and the
// kernel. Also draws a button that starts/pauses/resumes the rendering itself.
class RenderSettings {
public:
	RenderSettings(SceneEvents &sceneEvents, Console &console);
	void
	draw(Program &program);

private:
	void
	sceneLoadPopup(clRenderer::Scene &scene, clRenderer::Renderer &renderer);
	void
		kernelSelectPopup(clRenderer::Renderer &renderer);
	void
	setResolutionPopup(Program &program);
	bool sceneLoadPopupOpen,resolutionPopupOpen,newKernelPopupOpen;
	SceneEvents *sceneEvents;
	Console *console;
	// Remembers currently selected platform. Is needed when an user just
	// selected a platform but not a device because in that case it's not yet
	// stored in the renderer.
	const clRenderer::PlatformInfo *currPlatform = nullptr;
};
} // namespace gui::sceRen
} // namespace brdfEditor
#endif // !BRDFEDITOR_GUI_SCENE_RENDERING_RENDER_SETTINGS_HEADER
