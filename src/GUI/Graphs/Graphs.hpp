#ifndef BRDFEDITOR_GUI_GRAPHS_GRAPHS_HEADER
#define BRDFEDITOR_GUI_GRAPHS_GRAPHS_HEADER

#include <array>
#include <optional>

#include <src/GUI/GUIParams.hpp>
#include <src/GUI/SceneEvents.hpp>
#include <src/OpenCLRenderer/Renderer/Renderer.hpp>
#include <src/OpenCLRenderer/Scene.hpp>
#include "Graph.hpp"

namespace brdfEditor::gui::graphs {

// Offers IO create,inspect and manipulate graphs created from BRDFs.
class Graphs {
public:
	Graphs(clRenderer::Renderer &renderer,SceneEvents& sceneEvents, clRenderer::Scene &scene,
		   const GUIParams &parameters);
	void
	draw();

private:
	// Content of a popup that can create a new graph.
	void
	drawNewGraphPopup();
	const GUIParams *params;
	clRenderer::Renderer *renderer;
	clRenderer::Scene *scene;
	SceneEvents* sceneEvents;

	bool newGraphPopupOpen;
	std::vector<std::unique_ptr<Graph>> graphs;
};
} // namespace brdfEditor::gui::graphs
#endif // !BRDFEDITOR_GUI_GRAPHS_GRAPHS_HEADER
