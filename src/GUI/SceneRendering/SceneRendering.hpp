#ifndef BRDFEDITOR_GUI_SCENE_RENDERING_SCR_HEADER
#define BRDFEDITOR_GUI_SCENE_RENDERING_SCR_HEADER

#include <src/GUI/TwoPanelLayout.hpp>
#include <src/GUI/SceneEvents.hpp>

#include "Materials.hpp"
#include "RenderSettings.hpp"
#include "SceneTools.hpp"
#include "SceneView.hpp"

namespace brdfEditor {
class Program;
}
namespace brdfEditor::gui::sceRen {

// Responsible for drawing the SceneRendering GUI.
// Contains the rendered scene, renderer settings and scene tools.
// Also shows all materials with their parameter which can be tweaked via UI.
class SceneRendering : public TwoPanelLayout {
public:
	SceneRendering(Program &p, SceneEvents &sceneEvents, Console &console,
				   GUIParams &parameters);

private:
	Program *p;
	RenderSettings renderSettings;
	Materials materials;
	SceneTools sceneTools;
	SceneView sceneView;
	virtual void
	leftPanelContent() override final;
	virtual void
	mainPanelContent() override final;
};
} // namespace brdfEditor::gui::sceRen
#endif