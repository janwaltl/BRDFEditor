#include "Graph.hpp"

#include <array>
#include <algorithm>
#include <limits>
#include <optional>
#include <random>
#include <sstream>
#include <vector>

#include <ImGui/imgui.h>
#include <glm/gtc/matrix_transform.hpp>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <Imgui/imgui_internal.h>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/trigonometric.hpp>

#include <src/GUI/CommonElements.hpp>
#include <src/OpenCLRenderer/Renderer/Renderer.hpp>
#include <src/OpenCLRenderer/Scene.hpp>

namespace brdfEditor::gui::graphs {

namespace {
using maybeMatHandle_t = std::optional<clRenderer::MaterialHandle>;

Graph2D::plottedDataFlags
drawPlottedData(Graph2D::plottedDataFlags current) {
	using f = Graph2D::plottedDataFlags;
	bool red = current & f::red;
	bool green = current & f::green;
	bool blue = current & f::blue;
	bool brightness = current & f::brightness;
	bool pdf = current & f::pdf;

	ImGui::BeginColumns("PlottedDataCheckboxes", 2, ImGuiColumnsFlags_NoResize);
	ImGui::Checkbox("Red", &red);
	ImGui::Checkbox("Green", &green);
	ImGui::NextColumn();
	ImGui::Checkbox("Blue", &blue);
	ImGui::NextColumn();
	ImGui::Checkbox("Brightness", &brightness);
	ImGui::NextColumn();
	ImGui::Checkbox("PDF", &pdf);
	ImGui::EndColumns();

	f newFlags = f::none;
	auto setFlag = [&newFlags](bool state, f flag) {
		if (state)
			newFlags = f(newFlags | flag);
	};

	setFlag(red, f::red);
	setFlag(green, f::green);
	setFlag(blue, f::blue);
	setFlag(brightness, f::brightness);
	setFlag(pdf, f::pdf);
	return newFlags;
}

} // namespace

Graph2D::Graph2D(clRenderer::Renderer &renderer, clRenderer::Scene &scene,
				 const clRenderer::MaterialHandle &materialHandle,
				 std::size_t graphID) :
	renderer(&renderer),
	scene(&scene),
	matHandle(materialHandle),
	graphID(graphID),
	rescaleNextFrame(true) {

	settings.dataFlags = plottedDataFlags::all;
	settings.incAzim = -glm::pi<float>();
	settings.incElev = glm::quarter_pi<float>();
	settings.outAzim = 0.0f;
	settings.normal = {0.0f, 1.0f, 0.0f};
	settings.right = {1.0f, 0.0f, 0.0f};
	settings.numSamples = 1000;
	settings.opened = false;

	recompile();
	recompute();
}

void
Graph2D::draw(float canvasSizeX, float canvasSizeY) {
	auto pos = ImGui::GetCursorScreenPos();
	ImVec2 size{canvasSizeX, canvasSizeY};
	ImGui::PushClipRect(pos, pos + size, true);
	drawCanvas(pos, size);
	drawGraphControls(pos, size);
	drawAxes(pos, size);
	drawGrid(pos, size, 10, data.scale);
	drawData(pos, size);

	drawGraphSettings();
	ImGui::PopClipRect();
}

const char *
Graph2D::name() {
	return matHandle->first.c_str();
}

const clRenderer::MaterialHandle &
Graph2D::getMaterial() const {
	return matHandle;
}

namespace {

cl::Buffer
genParamBuffer(clRenderer::Material &material, cl::Context &context) {
	std::vector<clRenderer::BRDFParam_cl> paramBuffer;
	paramBuffer.resize(material.params.size());

	std::size_t i = 0;
	for (auto &&[paramName, param] : material.params) {
		paramBuffer[i++] = param.getCL();
	}
	if (paramBuffer.size() > 0)
		return cl::Buffer(context,
						  CL_MEM_READ_ONLY | CL_MEM_HOST_NO_ACCESS |
							  CL_MEM_COPY_HOST_PTR,
						  paramBuffer.size() * sizeof(clRenderer::BRDFParam_cl),
						  paramBuffer.data());
	else
		return cl::Buffer{};
}

cl::Buffer
genRNGBuffer(cl::Context &context, std::size_t size) {

	std::vector<cl_uint2> rngState(size);

	std::mt19937_64 eng(std::random_device{}());
	std::uniform_int_distribution<std::uint64_t> rng;
	for (std::size_t i = 0; i < rngState.size(); ++i) {
		std::uint64_t val = rng(eng);
		rngState[i].x = val >> 32ULL;
		rngState[i].y = val & 0xFFFFFFFF;
	}
	return cl::Buffer(context,
					  CL_MEM_READ_ONLY | CL_MEM_HOST_NO_ACCESS |
						  CL_MEM_COPY_HOST_PTR,
					  rngState.size() * sizeof(cl_uint2), rngState.data());
}
} // namespace

void
Graph2D::recompute() {
	auto &&[matName, material] = *matHandle;
	clParams = genParamBuffer(material, kernel->getContext());
	cl_float3 clNormal{settings.normal.x, settings.normal.y, settings.normal.z};
	auto up = glm::cross(settings.normal, settings.right);
	auto anglesToWorldVec =
		[&s = std::as_const(settings)](glm::vec3 up, float elev, float azim) {
			auto horizontal = glm::cos(azim) * s.right + glm::sin(azim) * up;
			return glm::cos(elev) * horizontal + glm::sin(elev) * s.normal;
		};
	auto wIn = anglesToWorldVec(up, settings.incElev, settings.incAzim);

	cl_float3 clRight{settings.right.x, settings.right.y, settings.right.z};
	cl_float3 clUp{up.x, up.y, up.z};

	cl_float3 clwIn{wIn.x, wIn.y, wIn.z};

	if (kernel->isValid()) {
		// Output buffer
		auto clOutput = cl::Buffer(kernel->getContext(),
								   CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY,
								   settings.numSamples * sizeof(cl_float4));
		auto &clKernel = kernel->getKernel();
		clKernel.setArg(0, clOutput);
		clKernel.setArg(1, clNormal);
		clKernel.setArg(2, clRight);
		clKernel.setArg(3, clUp);
		clKernel.setArg(4, clwIn);
		clKernel.setArg(5, settings.outAzim);
		clKernel.setArg(6, (cl_int)settings.includeCosTerm);
		clKernel.setArg(7, clParams);

		kernel->run(cl::NDRange(settings.numSamples));
		data.samples.resize(settings.numSamples);
		kernel->getQueue().enqueueReadBuffer(
			clOutput, false, 0, settings.numSamples * sizeof(data.samples[0]),
			data.samples.data());
	}
	kernel->getQueue().finish();
}

void
Graph2D::recompile() {
	using namespace clRenderer;
	auto &&[matName, material] = *matHandle;
	auto &&[brdfName, brdf] = *material.brdf;

	std::string getPDFSource =
		brdf.hasCustomSampling()
			? *brdf.getGetwInPDFFncName() + "(normal, wOut, wIn,matParams);"
			: R"(dot(wIn,normal)/PI;)";

	std::string kernelSource;
	kernelSource += R"(
#include "Common/Constants.cl"
#include "Common/Geometry.cl"
#include "Common/Random.cl"
#include "Common/Types.cl"

)";
	kernelSource += brdf.getDeclarations();
	kernelSource +=
		R"(
__kernel void compute(__global float4* result, float3 normal, float3 right, float3 up, float3 wOut, float outAzim,int includeCosTermBool, global Param* matParams) {
    // Get the index of the current element to be processed
    int i = get_global_id(0);
	float outElev =   (float)(i)/get_global_size(0) *PI;
	
	float3 horizontal = normalize(cos(outAzim) * right + sin(outAzim) * up);
	float3 wIn = normalize(cos(outElev) * horizontal + sin(outElev) * normal);

	float3 brdf = )" +
		brdf.getBRDFFncName() +
		R"((normal,wOut,wIn,matParams);
	if(includeCosTermBool!=0)
        brdf *= dot(wIn,normal);
	result[i].xyz=brdf;
	result[i].w= )" +
		getPDFSource +
		R"(//(0.2126f*brdf.x + 0.7152f*brdf.y + 0.0722f*result[i].z);
}
)";
	kernelSource += brdf.getDefinitions();

	kernel = renderer->genCustomKernel(kernelSource, "compute");
}

namespace {

// Draws a circle widget which allows user to change incAzim,incElev and
// outAzim.
// - incAzim, incElve, outAzim: values in [-pi,pi], serve as both input and
// output. Returns: whether any of the input values have been changed
bool
drawDirectionsPicker(float &incAzim, float &incElev, float &outAzim) {
	const glm::vec2 p = ImGui::GetCursorScreenPos();
	const ImColor black{0.0f, 0.0f, 0.0f, 1.0f};
	const ImColor blue{0.0f, 0.0f, 1.0f, 1.0f};
	const ImColor red{1.0f, 0.0f, 0.0f, 1.0f};
	const float pickerRadius{32.0f};
	const ImVec2 pickerCenter{p + pickerRadius};
	const ImVec2 pickerSize{pickerRadius * 2.0f, pickerRadius * 2.0f};

	bool anyChanged = false;
	ImDrawList *drawList = ImGui::GetWindowDrawList();
	drawList->AddCircle(p + pickerRadius, pickerRadius, black, 32, 2.0f);

	ImGui::InvisibleButton("Picker", pickerSize);

	glm::vec2 incScreenPos;

	// Compute new incoming direction
	if (ImGui::IsItemActive()) {
		incScreenPos = ImGui::GetMousePos();
		glm::vec2 incOffset = incScreenPos - pickerCenter;
		float incLen = glm::length(incOffset);
		if (glm::epsilonNotEqual(incLen, 0.0f, glm::epsilon<float>())) {
			// Clamp to the circle
			if (incLen > pickerRadius) {
				incOffset *= pickerRadius / incLen;
				incScreenPos = pickerCenter + incOffset;
				incLen = pickerRadius;
			}
			incAzim = glm::atan(incOffset.y, incOffset.x);
			anyChanged = true;
		}
		incElev = glm::acos(incLen / pickerRadius);
	} else {
		incScreenPos =
			pickerCenter + pickerRadius * glm::cos(incElev) *
							   glm::vec2{glm::cos(incAzim), glm::sin(incAzim)};
	}

	drawList->AddLine(pickerCenter, incScreenPos, blue, 2.0f);

	// Compute new outgoing direction with right click
	glm::vec2 outOffset;
	if (ImGui::IsItemHovered() && ImGui::IsMouseDown(1)) {
		outOffset = ImGui::GetMousePos() - pickerCenter;
		float outLen = glm::length(outOffset);
		if (glm::epsilonNotEqual(outLen, 0.0f, glm::epsilon<float>())) {
			outOffset = outOffset / outLen * pickerRadius;
			outAzim = glm::atan(outOffset.y, outOffset.x);
		}
		anyChanged = true;
	} else
		outOffset =
			pickerRadius * glm::vec2{glm::cos(outAzim), glm::sin(outAzim)};

	drawList->AddLine(pickerCenter - outOffset, pickerCenter + outOffset, red,
					  2.0f);
	drawList->AddCircleFilled(pickerCenter + outOffset, 5.0f, red);
	return anyChanged;
}
bool
drawCosTermCheckbox(bool &includeCosTerm) {
	// TODO Add help here
	return ImGui::Checkbox("Include the cos term", &includeCosTerm);
}

// Draws UI elements that allow for precisely setting all graph parameter -
// numSamples, normal, incoming and outgoing directions.
bool
drawGraphParameters(int &numSamples, float &inElev, float &inAzim,
					float &outAzim) {
	bool active = false;
	active |= ImGui::InputInt("# Samples", &numSamples, 10, 100);

	ImGui::PushItemWidth(200);

	float inElevDeg = glm::degrees(inElev);
	float inAzimDeg = glm::degrees(inAzim);
	float outAzimDeg = glm::degrees(outAzim);
	active |= ImGui::SliderFloat("Incoming elevation angle [0,90]", &inElevDeg,
								 0.0f, 90.0f);
	active |= ImGui::SliderFloat("Incoming azimuth angle [-180,180]",
								 &inAzimDeg, -180.0f, 180.0f);
	active |= ImGui::SliderFloat("Outgoing azimuth angle [-180,180]",
								 &outAzimDeg, -180.0f, 180.0f);
	inElev = glm::radians(inElevDeg);
	inAzim = glm::radians(inAzimDeg);
	outAzim = glm::radians(outAzimDeg);

	ImGui::PopItemWidth();
	return active;
}
} // namespace

void
Graph2D::drawGraphControls(ImVec2 pos, ImVec2 size) {
	if (ImGui::Button("Rescale"))
		this->scaleData();
	ImGui::SameLine();
	if (ImGui::Button("Settings")) {
		ImGui::SetNextWindowFocus();
		ImGui::SetNextWindowPos(ImGui::GetCursorPos(), ImGuiCond_Once);
		settings.opened = true;
	}

	if (drawDirectionsPicker(settings.incAzim, settings.incElev,
							 settings.outAzim))
		recompute();

	if (!ImGui::IsItemHovered() && ImGui::IsWindowHovered() &&
		ImGui::IsMouseHoveringRect(pos, pos + size))
		drawHoveredValues(pos, size);

	drawIncomingDir(pos, size);
}

void
Graph2D::drawGraphSettings() {
	if (!settings.opened)
		return;
	auto winName = this->name() + std::string(":Settings");
	ImGui::PushID(graphID);
	if (ImGui::Begin(winName.c_str(), &settings.opened,
					 ImGuiWindowFlags_AlwaysAutoResize |
						 ImGuiWindowFlags_NoCollapse)) {

		maybeMatHandle_t newSelected = matHandle;
		ImGui::PushItemWidth(200);
		drawMaterialsCombo(*scene, newSelected);
		ImGui::PopItemWidth();
		ImGui::SameLine();
		ImGui::Text("Materials");
		if (newSelected) {
			bool recomp = matHandle != *newSelected;
			matHandle = *newSelected;
			if (recomp) {
				recompile();
				recompute();
			}
		}

		settings.dataFlags = drawPlottedData(settings.dataFlags);

		if (drawCosTermCheckbox(settings.includeCosTerm))
			recompute();
		if (drawGraphParameters(settings.numSamples, settings.incElev,
								settings.incAzim, settings.outAzim))
			recompute();
		ImGui::Separator();
		if (drawMatParams(matHandle))
			recompute();
		if (ImGui::Button("Rescale"))
			this->scaleData();
	}
	ImGui::End();
	ImGui::PopID();
}

void
Graph2D::drawCanvas(ImVec2 pos, ImVec2 size) {
	ImDrawList *drawList = ImGui::GetWindowDrawList();
	// Draw canvas
	drawList->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y),
							ImGui::GetColorU32(ImGuiCol_FrameBg));
}

void
Graph2D::drawAxes(ImVec2 pos, ImVec2 size) {
	ImDrawList *drawList = ImGui::GetWindowDrawList();

	drawList->AddLine(ImVec2(pos.x, pos.y + size.y),
					  ImVec2(pos.x + size.x, pos.y + size.y),
					  ImGui::GetColorU32(ImGuiCol_Border), 2.0f);

	drawList->AddLine(ImVec2(pos.x + size.x / 2.0f, pos.y),
					  ImVec2(pos.x + size.x / 2.0f, pos.y + size.y),
					  ImGui::GetColorU32(ImGuiCol_Border), 2.0f);
}

void
Graph2D::drawIncomingDir(ImVec2 pos, ImVec2 size) {
	ImVec2 origin = ImVec2(pos.x + size.x / 2.0f, pos.y + size.y);

	float length = glm::min(size.x, size.y) / 4.0f;

	float azimutDiff = settings.incAzim - settings.outAzim;

	ImVec2 incDir =
		ImVec2(glm::cos(azimutDiff) * glm::cos(settings.incElev) * length,
			   glm::sin(settings.incElev) * length);

	ImVec2 incPos = incDir;
	incPos.y *= -1;
	incPos += origin;

	ImDrawList *drawList = ImGui::GetWindowDrawList();

	// The more affected by azimut the greener.
	float azEffect = glm::abs(glm::sin(azimutDiff));
	drawList->AddLine(origin, incPos,
					  ImGui::ColorConvertFloat4ToU32(
						  ImVec4(0.0f, azEffect, 1.0f - azEffect, 1.0f)),
					  5.0f);
}

void
Graph2D::drawGrid(ImVec2 pos, ImVec2 size, std::size_t numLines,
				  float dataScale) {

	ImDrawList *drawList = ImGui::GetWindowDrawList();
	const auto color = ImGui::GetColorU32(ImGuiCol_Border, 0.3f);
	const auto textColor = ImGui::GetColorU32(ImGuiCol_Text);
	const ImVec2 origin{pos.x + size.x / 2.0f, pos.y + size.y};

	const auto radius = [&](std::size_t i) {
		return glm::max(size.x / 2.0f, size.y) * i / numLines;
		;
	};
	// Concentric circles.
	for (std::size_t i = 0; i < numLines; ++i) {
		// Scale lines according to the bigger size = fills screen.

		constexpr const std::size_t numSegments = 32;
		std::vector<ImVec2> arc(numSegments + 1, origin);
		for (std::size_t k = 0; k <= numSegments; ++k) {
			float angle = k / (float)numSegments * glm::pi<float>();
			arc[k] = origin +
					 radius(i) * glm::vec2{glm::cos(angle), -glm::sin(angle)};
		}
		drawList->AddPolyline(arc.data(), numSegments + 1, color, false, 1.0f);

		auto textPos = origin;
		textPos.y -= radius(i);

		std::array<char, 32> text;
		std::snprintf(text.data(), text.size(), "%.2f", radius(i) / dataScale);
		drawList->AddText(textPos, textColor, text.data());
	}

	// Lines
	std::array<float, 3> angles = {22.5f, 45.f, 67.5f};
	for (float a : angles) {
		float aRad = glm::radians(a);
		float r = radius(numLines - 1);
		std::array<char, 32> text;
		std::snprintf(text.data(), text.size(), "%.1f", a);

		auto lineOffset = r * glm::vec2{glm::cos(aRad), -glm::sin(aRad)};
		drawList->AddLine(origin, origin + lineOffset, color);
		drawList->AddText(origin + lineOffset, textColor, text.data());
		lineOffset.x *= -1;
		drawList->AddLine(origin, origin + lineOffset, color);
		drawList->AddText(origin + lineOffset, textColor, text.data());
	}
}
namespace {
struct GraphPoints {
	std::vector<ImVec2> points;
	bool show;
	ImU32 color;
};
struct MinMaxPoints {
	glm::vec2 minPoint, maxPoint;
};

// Converts sampled value and its index to either spherical or cartesian
// coordinates.
glm::vec2
convertSampleToCoords(std::size_t i, float value, std::size_t numSamples) {
	//[0,numSamples) -> [0,180deg]
	double angle = i / (double)numSamples * glm::pi<double>();
	return {(float)glm::cos(angle) * value, (float)glm::sin(angle) * value};
}

// Converts samples to shown graphChannels.points using 'convertSampleToCoords'.
// Only converts shown points and returns minimal and maximal point.
// - graphChannels: properly initialized array with correct show flag.
// Returns: graphPoints, minMax
void
convertSamplesToCoords(const std::vector<cl_float4> &samples,
					   std::array<GraphPoints, 5> &graphChannels,
					   MinMaxPoints &minMax) {
	auto &min = minMax.minPoint;
	auto &max = minMax.maxPoint;

	max.x = max.y = std::numeric_limits<float>::min();
	min.x = min.y = std::numeric_limits<float>::max();

	const std::size_t numSamples = samples.size();

	for (auto &channel : graphChannels)
		channel.points.reserve(numSamples);
	for (std::size_t i = 0; i < numSamples; ++i) {
		// Convert RGB+PDF to RGB+PDF+Brightness
		float sample[5] = {samples[i].x, samples[i].y, samples[i].z,
						   samples[i].w,
						   0.2126f * samples[i].x + 0.7152f * samples[i].y +
							   0.0722f * samples[i].z};
		for (std::size_t k = 0; k < graphChannels.size(); ++k)
			if (graphChannels[k].show) {
				auto point = convertSampleToCoords(i, sample[k], numSamples);
				graphChannels[k].points.push_back(point);
				max = glm::max(max, point);
				min = glm::min(min, point);
			}
	}
}
// Converts graphChannels.points to screen coordinates. Only converts shown
// channels.
// - graphPoints: properly initialized array with shown channels.
// Returns: scale from graphCoords to screenCoords
float
convertGraphPointToScreenCoords(const glm::vec2 &canvasPos,
								const glm::vec2 &canvasSize,
								std::size_t numSamples, const glm::vec2 extent,
								std::array<GraphPoints, 5> &graphChannels) {

	auto scale = canvasSize / extent;
	auto minScale = glm::min(scale.x, scale.y);

	auto scalePoint = [&](glm::vec2 point) {
		// Y goes down in imGui screen coords
		point.y *= -1.0f;

		point *= minScale;

		point += canvasPos;

		// Center the graph

		point += glm::vec2{canvasSize.x / 2.0f, canvasSize.y};
		return point;
	};
	for (std::size_t k = 0; k < graphChannels.size(); ++k)
		for (std::size_t i = 0; i < numSamples; ++i)
			if (graphChannels[k].show) {
				auto &point = graphChannels[k].points[i];
				graphChannels[k].points[i] = scalePoint(point);
			}
	return minScale;
}
} // namespace

void
Graph2D::drawData(ImVec2 pos, ImVec2 size) {

	const std::size_t numSamples = data.samples.size();
	auto plot = [flags = settings.dataFlags](plottedDataFlags flag) {
		return flags & flag;
	};

	std::array<GraphPoints, 5> graphChannels;

	graphChannels[0].show = plot(plottedDataFlags::red);
	graphChannels[1].show = plot(plottedDataFlags::green);
	graphChannels[2].show = plot(plottedDataFlags::blue);
	graphChannels[3].show = plot(plottedDataFlags::pdf);
	graphChannels[4].show = plot(plottedDataFlags::brightness);
	graphChannels[0].color =
		ImGui::ColorConvertFloat4ToU32({1.0f, 0.0f, 0.0f, 1.0f});
	graphChannels[1].color =
		ImGui::ColorConvertFloat4ToU32({0.0f, 1.0f, 0.0f, 1.0f});
	graphChannels[2].color =
		ImGui::ColorConvertFloat4ToU32({0.0f, 0.0f, 1.0f, 1.0f});
	graphChannels[3].color =
		ImGui::ColorConvertFloat4ToU32({0.0f, 0.0f, 0.0f, 1.0f});
	graphChannels[4].color =
		ImGui::ColorConvertFloat4ToU32({1.0f, 1.0f, 0.0f, 1.0f});

	MinMaxPoints minMax;

	convertSamplesToCoords(data.samples, graphChannels, minMax);

	auto extent = minMax.maxPoint - minMax.minPoint;
	if (rescaleNextFrame) {
		data.extent = extent;
		rescaleNextFrame = false;
	}

	data.scale = convertGraphPointToScreenCoords(pos, size, numSamples,
												 data.extent, graphChannels);

	ImDrawList *drawList = ImGui::GetWindowDrawList();
	for (std::size_t k = 0; k < graphChannels.size(); ++k)
		if (graphChannels[k].show)
			drawList->AddPolyline(graphChannels[k].points.data(), numSamples,
								  graphChannels[k].color, false, 2.0f);
}
void
Graph2D::scaleData() {
	rescaleNextFrame = true;
}

void
Graph2D::drawHoveredValues(glm::vec2 pos, glm::vec2 size) {
	glm::vec2 mouseCoords = ImGui::GetMousePos();
	mouseCoords -= glm::vec2{size.x / 2.0f, size.y};
	mouseCoords -= pos;
	mouseCoords /= data.scale;
	mouseCoords.y *= -1;

	float elevRad = glm::atan(mouseCoords.y, mouseCoords.x);
	ImGui::Text(" Outgoing elevation: %.1f°deg", glm::degrees(elevRad));
	ImGui::Text(" Value: %.2f ", glm::length(mouseCoords));
}

bool
Graph::isHidden() const {
	return hidden;
}
void
Graph::setOpen(bool open) {
	hidden = !open;
}
} // namespace brdfEditor::gui::graphs
