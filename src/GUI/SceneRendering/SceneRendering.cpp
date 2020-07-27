#include "SceneRendering.hpp"

#include <src/Core/Program.hpp>

namespace brdfEditor::gui::sceRen {
SceneRendering::SceneRendering(Program &p, SceneEvents &sceneEvents,
							   Console &console, GUIParams &parameters) :
	TwoPanelLayout(console, parameters,"Control Panel","Scene"),
	p(&p),
	renderSettings(sceneEvents, this->parameters().getConsole()),
	materials(p.getScene()),
	sceneTools(p.getScene()),
	sceneView(p.getScene()) {}

void
SceneRendering::leftPanelContent() {
	renderSettings.draw(*p);
	materials.draw(p->getRenderer());
	sceneTools.draw(p->getRenderer());
}

void
SceneRendering::mainPanelContent() {
	sceneView.draw(*p, parameters());
}

} // namespace brdfEditor::gui::sceRen