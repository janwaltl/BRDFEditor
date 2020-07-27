#ifndef BRDFEDITOR_GUI_SCENE_RENDERING_SCENE_VIEW_HEADER
#define BRDFEDITOR_GUI_SCENE_RENDERING_SCENE_VIEW_HEADER

#include <future>
#include <optional>

#include <glm/glm.hpp>

#include <src/GUI/GUIParams.hpp>
#include <src/OpenCLRenderer/Camera.hpp>

namespace brdfEditor {
class Program;
namespace clRenderer {
class Scene;
class Renderer;
} // namespace clRenderer
namespace gui {
class TwoPanelParams;
namespace sceRen {

// Draws the renderer's output image, UI elements that show rendering statics.
// Also controls the scene's camera using a mouse trackball.
class SceneView {
public:
	SceneView(clRenderer::Scene &scene);
	// Assumes that the scene passed in the constructor is the same as the one
	// in the program. Will send a recompilation flags to the render when the
	// user moved the camera.
	void
	draw(Program &p, const TwoPanelParams &panelParams);

private:
	struct Trackball // Data for trackball control.
	{
		bool isDragging;
		// Cached camera before the start of rotating. Used for computing the
		// rotation vectors based on dragged distance and direction.
		clRenderer::Camera cachedCamera;
		glm::mat4 invView;
	};
	// Controls the rendered scene with trackbal
	// --imgPos: Pixel coordinates of the top-left corner of the image
	// --imRes: Pixel resolution of the image
	void
	trackballControl(clRenderer::Camera &cam, clRenderer::Renderer &renderer,
					 const glm::vec2 &imgPos, const glm::vec2 &imRes);

	void
	imageSaving(Program &program, const TwoPanelParams &parameters);
	clRenderer::Scene *scene;
	Trackball trackball;
	std::optional<std::future<bool>> maybeSavedImage;
};
} // namespace sceRen
} // namespace gui
} // namespace brdfEditor
#endif // !BRDFEDITOR_GUI_SCENE_RENDERING_SCENE_VIEW_HEADER
