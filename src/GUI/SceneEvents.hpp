#ifndef BRDREDITOR_GUI_SCENE_EVENTS_HEADER
#define BRDREDITOR_GUI_SCENE_EVENTS_HEADER

#include <functional>
#include <vector>

namespace brdfEditor::gui {

// Simple event system that informs subscribers about changes in the scene.
class SceneEvents {
public:
	using matRemovedHandler_t = std::function<void(const std::string &)>;
	using sceneReloadedHandler_t = std::function<void(void)>;
	using brdfSourceChangedHandler_t = std::function<void(const std::string &)>;
	// Calls all subscribers that a material is about to be removed by
	// scene::removeMaterial. Users should behave as if that operation will
	// succeed. Caller should ensure that it does.
	void
	raiseMatRemovalEvent(const std::string &matNamToRemove) const;
	void
	raiseSceneReloadedEvent() const;
	void raiseBRDFSourceChanged(const std::string& name) const;
	void
	subscribeToMatRemoval(matRemovedHandler_t handler);
	void
	subscribeToSceneReloading(sceneReloadedHandler_t handler);
	void subscribeBRDFSourceChange(brdfSourceChangedHandler_t handl);
private:
	std::vector<matRemovedHandler_t> matRemovedHandlers;
	std::vector<matRemovedHandler_t> sourceChangedHandlers;
	std::vector<sceneReloadedHandler_t> sceneReloadedHandlers;
};
} // namespace brdfEditor::gui
#endif // !BRDREDITOR_GUI_SCENE_EVENTS_HEADER
