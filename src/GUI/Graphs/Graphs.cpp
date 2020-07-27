#include "Graphs.hpp"

#include <string>

#include <ImGui/imgui.h>

#include <src/GUI/CommonElements.hpp>

namespace brdfEditor::gui::graphs {

namespace {

struct rowCols {
	std::size_t rows, cols;
};
struct TopBarRequests {
	std::optional<rowCols> setGrid;
	bool openNewGraph = false;
	std::optional<std::size_t> showGraphI;
};
std::optional<rowCols>
drawOrganizeToGrid() {
	ImGui::Text("Layout: ");
	std::optional<rowCols> rowCols;
	;
	auto button = [&](std::size_t r, std::size_t c) {
		ImGui::SameLine();
		char buff[4];
		std::snprintf(buff, sizeof(buff), "%dx%d", r, c);
		if (ImGui::Button(buff))
			rowCols = {r, c};
	};
	button(1, 1);
	button(1, 2);
	button(2, 2);
	button(2, 3);
	button(3, 3);
	return rowCols;
}

// Draws top bar with list of all graphs and ability to remove them.
//--graphs: graphs to be listed, user might delete some graphs.
TopBarRequests
drawTopBar(std::vector<std::unique_ptr<Graph>> &graphs) {

	TopBarRequests requests;
	auto barWindowFlags = ImGuiWindowFlags_NoTitleBar |
						  ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	if (ImGui::Begin("Top bar", nullptr, barWindowFlags)) {
		ImGui::AlignFirstTextHeightToWidgets();
		if (ImGui::Button("New graph"))
			requests.openNewGraph = true;
		ImGui::SameLine();
		if (ImGui::BeginCombo("##Graphs", "List of Graphs")) {
			for (std::size_t i = 0; i < graphs.size();) {
				auto &&graph = graphs[i];
				ImGui::Text("#%d: %s", i + 1, graph->name());
				ImGui::SameLine();
				if (ImGui::Button("Show"))
					requests.showGraphI = i;
				ImGui::SameLine();
				if (ImGui::Button("Remove"))
					graphs.erase(graphs.begin() + i);
				else
					++i;
			}
			ImGui::EndCombo();
		}
		ImGui::SameLine();
		requests.setGrid = drawOrganizeToGrid();
	}
	ImGui::End();
	return requests;
}
// Positions the graph window based on the given grid and the window's index.
void
positionNextWindow(const rowCols &grid, const glm::vec2 &canvasPos,
				   const glm::vec2 &canvasSize, std::size_t windIndex) {
	std::size_t column = windIndex % grid.cols;
	std::size_t row = (windIndex / grid.cols) % grid.rows;

	auto winSize = canvasSize / glm::vec2{grid.cols, grid.rows};
	auto winPos = canvasPos + winSize * glm::vec2(column, row);

	ImGui::SetNextWindowPos(winPos, ImGuiCond_Always);
	ImGui::SetNextWindowSize(winSize, ImGuiCond_Always);
}
//--maxWinPos,maxWinSize: Maximum dimensions of graph windows if they are
// maximized.
//--graphs: Graphs to draw.
//--toGrid: Organize all graph windows to 2X2 grid.
//--bringForwardI: brings the graphs[I]'s window forward.
void
drawGraphs(glm::vec2 maxWinPos, glm::vec2 maxWinSize,
		   std::vector<std::unique_ptr<Graph>> &graphs,
		   const TopBarRequests &requests) {
	// How many visible graph did we draw.
	std::size_t visibleIndex = 0;
	for (std::size_t i = 0; i < graphs.size(); ++i) {
		auto &&graph = graphs[i];

		if (requests.showGraphI == i) {
			graph->setOpen(true);
			ImGui::SetNextWindowCollapsed(false, ImGuiCond_Always);
			ImGui::SetNextWindowFocus();
		}

		if (graph->isHidden())
			continue;

		std::string winName = "#";
		winName += std::to_string(i + 1) + ": " + graph->name() + "###" +
				   std::to_string(i + 1);
		ImGui::SetNextWindowPos(maxWinPos, ImGuiCond_Once);
		ImGui::SetNextWindowSize(maxWinSize, ImGuiCond_Once);
		if (requests.setGrid)
			positionNextWindow(*requests.setGrid, maxWinPos, maxWinSize,
							   visibleIndex);
		bool open = true;

		if (ImGui::Begin(winName.c_str(), &open, ImGuiWindowFlags_NoCollapse))
			graph->draw(ImGui::GetContentRegionAvail().x,
						ImGui::GetContentRegionAvail().y);
		ImGui::End();
		graph->setOpen(open);
		++visibleIndex;
	}
}
} // namespace

Graphs::Graphs(clRenderer::Renderer &renderer, SceneEvents &sceneEvents,
			   clRenderer::Scene &scene, const GUIParams &parameters) :
	params(&parameters),
	renderer(&renderer),
	scene(&scene),
	sceneEvents(&sceneEvents),
	newGraphPopupOpen(false) {

	// Remove any graphs with removed material
	sceneEvents.subscribeToMatRemoval([this](
										  const std::string &matRemovedName) {
		auto removeMat = [&matRemovedName](auto &&graphPtr) {
			return graphPtr->getMaterial()->first == matRemovedName;
		};
		graphs.erase(std::remove_if(graphs.begin(), graphs.end(), removeMat),
					 graphs.end());
	});
	// Recompile any graphs if the underlying source changed
	sceneEvents.subscribeBRDFSourceChange([this](const std::string &brdfName) {
		for (auto &&g : this->graphs) {
			auto &&graphBRDFName = g->getMaterial()->second.brdf->first;
			if (brdfName == graphBRDFName) {
				auto &&graph2D = dynamic_cast<Graph2D *>(g.get());
				if (graph2D)
					graph2D->recompile();
			}
		}
	});
	// Simply delete all graphs
	sceneEvents.subscribeToSceneReloading([this]() { graphs.clear(); });
}

void
Graphs::draw() {

	if (newGraphPopupOpen)
		ImGui::OpenPopup("NewGraphPopup");
	drawNewGraphPopup();

	ImVec2 barWindowSize{params->canvasSize().x,
						 1.5f * ImGui::GetTextLineHeightWithSpacing()};

	ImGui::SetNextWindowPos(params->canvasPos(), ImGuiCond_Always);
	ImGui::SetNextWindowSize(barWindowSize, ImGuiCond_Always);

	TopBarRequests requests = drawTopBar(graphs);
	if (requests.openNewGraph)
		newGraphPopupOpen = true;
	auto maxWinPos = params->canvasPos();
	maxWinPos.y += barWindowSize.y;
	auto maxWinSize = params->canvasSize();
	maxWinSize.y -= barWindowSize.y;
	drawGraphs(maxWinPos, maxWinSize, graphs, requests);
}
void
Graphs::drawNewGraphPopup() {
	// There can only be one popup active.
	static std::optional<clRenderer::MaterialHandle> selectedMaterial;
	static std::size_t nextFreeGraphID = 0;
	static std::optional<std::string> kernelFailed;
	if (ImGui::BeginPopupModal("NewGraphPopup", &newGraphPopupOpen,
							   ImGuiWindowFlags_AlwaysAutoResize)) {
		auto selMatStr = selectedMaterial ? (*selectedMaterial)->first.c_str()
										  : "Select a material";
		drawMaterialsCombo(*scene, selectedMaterial);

		bool close = false;
		if (selectedMaterial) {
			if (kernelFailed)
				ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f),
								   kernelFailed->c_str());

			if (ImGui::Button("Create")) {
				try {
					graphs.push_back(std::make_unique<Graph2D>(
						*renderer, *scene, *selectedMaterial,
						++nextFreeGraphID));
					close = true;
				} catch (const Exception &e) {
					kernelFailed =
						"Could not create the graph. Make sure that\n the"
						"function can be compiled in the editor.";
					kernelFailed = *kernelFailed + e.what();
				}
			}
			ImGui::SameLine();
		}
		if (ImGui::Button("Close Popup"))
			close = true;

		if (close) {
			kernelFailed.reset();
			newGraphPopupOpen = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}
} // namespace brdfEditor::gui::graphs