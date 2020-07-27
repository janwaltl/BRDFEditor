#include "SceneEvents.hpp"

void
brdfEditor::gui::SceneEvents::raiseMatRemovalEvent(
	const std::string &removedMatName) const {
	for (auto &handler : matRemovedHandlers)
		std::invoke(handler, removedMatName);
}

void
brdfEditor::gui::SceneEvents::raiseSceneReloadedEvent() const {
	for (auto &handler : sceneReloadedHandlers)
		std::invoke(handler);
}

void
brdfEditor::gui::SceneEvents::raiseBRDFSourceChanged(const std::string &name) const {
	for (auto &handler : sourceChangedHandlers)
		std::invoke(handler, name);
}

void
brdfEditor::gui::SceneEvents::subscribeToMatRemoval(
	matRemovedHandler_t handler) {
	matRemovedHandlers.push_back(std::move(handler));
}

void
brdfEditor::gui::SceneEvents::subscribeToSceneReloading(
	sceneReloadedHandler_t handler) {
	sceneReloadedHandlers.push_back(std::move(handler));
}

void
brdfEditor::gui::SceneEvents::subscribeBRDFSourceChange(brdfSourceChangedHandler_t handl) {
	sourceChangedHandlers.push_back(handl);
}
