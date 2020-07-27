#ifndef BRDFEDITOR_GUI_GRAPHS_GRAPH_HEADER
#define BRDFEDITOR_GUI_GRAPHS_GRAPH_HEADER

#include <memory>
#include <optional>
#include <vector>

#include <glm/glm.hpp>
#include <ImGui/imgui.h>

#include <src/OpenCLRenderer/CLHeaders.hpp>
#include <src/OpenCLRenderer/Materials/Material.hpp>
#include <src/OpenCLRenderer/Kernel/CustomKernel.hpp>

namespace brdfEditor::clRenderer {
class Renderer;
class Scene;
} // namespace brdfEditor::clRenderer
namespace brdfEditor::gui::graphs {
// Abstract class for a drawable, named graph
class Graph {
public:
	virtual ~Graph() = default;
	// Override to draw the graph, is called withing a ImGui window.
	virtual void
	draw(float canvasWidth, float canvasHeight) = 0;
	virtual const char *
	name() = 0;
	// Returns: Material's being plotted.
	virtual const clRenderer::MaterialHandle &
	getMaterial() const = 0;

	bool
	isHidden() const;
	void
	setOpen(bool open);

private:
	bool hidden = false;
};

// Draws a 2D line graph for passed material. Shows R,G,B responses for all
// possible output angles based on chosen input direction.
class Graph2D : public Graph {
public:
	// What to draw
	enum plottedDataFlags : uint8_t {
		brightness = 1 << 0,
		red = 1 << 1,
		green = 1 << 2,
		blue = 1 << 3,
		pdf = 1 <<4,
		all = brightness | red | green | blue | pdf,
		none = 0,
	};

	Graph2D(clRenderer::Renderer &renderer, clRenderer::Scene &scene,
			const clRenderer::MaterialHandle &materialHandle,
			std::size_t graphID);

	virtual void
	draw(float canvasSizeX, float canvasSizeY) override final;
	// Returns: Material's name.
	virtual const char *
	name() override final;
	virtual const clRenderer::MaterialHandle &
	getMaterial() const override final;

	void
	recompile();

private:
	void
	recompute();

	void
	drawGraphControls(ImVec2 pos, ImVec2 size);
	void
	drawGraphSettings();
	void
	drawHoveredValues(glm::vec2 pos, glm::vec2 size);
	void
	drawCanvas(ImVec2 pos, ImVec2 size);
	void
	drawAxes(ImVec2 pos, ImVec2 size);
	void
	drawIncomingDir(ImVec2 pos, ImVec2 size);
	void
	drawGrid(ImVec2 pos, ImVec2 size, std::size_t numLines, float dataScale);
	void
	drawData(ImVec2 pos, ImVec2 size);

	void
	scaleData();
	clRenderer::Renderer *renderer;
	clRenderer::Scene *scene;
	clRenderer::MaterialHandle matHandle;
	cl::Buffer clParams;
	std::shared_ptr<clRenderer::CustomKernel> kernel;

	const int graphID;

	struct {
		plottedDataFlags dataFlags = plottedDataFlags::all;
		int numSamples = 1000;
		glm::vec3 normal;
		glm::vec3 right;
		// In Radianas
		float incElev, incAzim;
		float outAzim;
		bool includeCosTerm = true;

		bool opened = false;
	} settings;
	struct GraphData {
		glm::vec2 extent;
		float scale;
		std::vector<cl_float4> samples;
		// float3 wOut + pdf
		std::vector<cl_float4> rngSamples;
	} data;
	bool rescaleNextFrame;
};
void
genParamBuffer(brdfEditor::clRenderer::Material &material);
} // namespace brdfEditor::gui::graphs
#endif // !BRDFEDITOR_GUI_GRAPHS_GRAPH_HEADER
