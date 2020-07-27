#include "SceneView.hpp"

#include <iomanip>
#include <chrono>
#include <sstream>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/component_wise.hpp>
#include <ImGui/imgui.h>

#include <src/Core/Program.hpp>
#include <src/GUI/TwoPanelLayout.hpp>

namespace brdfEditor::gui::sceRen {
SceneView::SceneView(clRenderer::Scene &scene) : scene(&scene) {
	trackball.isDragging = false;
}

namespace {
std::string
renderStringFromStats(std::size_t width, std::size_t height,
					  std::size_t numSamples, std::size_t milisecondsElapsed) {

	std::size_t mins = milisecondsElapsed / 1000 / 60;
	milisecondsElapsed -= mins * 60 * 1000;
	double secs = milisecondsElapsed / 1000.0;
	double avgTimePerSample =
		numSamples != 0 ? milisecondsElapsed / (double)numSamples : 0.0;
	std::stringstream ss;
	ss << "Resolution: ";
	ss << std::setw(4) << width;
	ss << 'x';
	ss << std::setw(4) << height;
	ss << " | ";
	ss << "Render time: ";
	ss << std::setw(3) << mins;
	ss << "m ";
	ss << std::setw(4) << std::fixed << std::setprecision(1) << secs;
	ss << "s | ";
	ss << "Samples: ";
	ss << std::setw(5) << numSamples;
	ss << " | ";
	ss << "Avg. time per sample: ";
	ss << std::setw(4) << std::fixed << std::setprecision(1)
	   << avgTimePerSample;
	ss << "ms";
	return ss.str();
}

struct TMArgsDrawVisitor : public clRenderer::ToneMappingKArgsVisitor {
	// Inherited via ToneMappingKArgsVisitor
	virtual void
	visit(clRenderer::SimpleTMKernel &kernel) override final {
		float beta = kernel.getBeta();
		ImGui::PushItemWidth(50);
		ImGui::DragFloat("Beta", &beta, 0.01f, 0.0f, 1000.0f);
		ImGui::PopItemWidth();
		kernel.setBeta(beta);
	}
	virtual void
	visit(clRenderer::GammaCompressionTMKernel &kernel) override final {
		ImGui::PushItemWidth(50);
		float gamma = kernel.getGamma();
		ImGui::DragFloat("Gamma", &gamma, 0.01f, 0.0f, 10.0f);
		kernel.setGamma(gamma);
		ImGui::SameLine();
		float alpha = kernel.getAlpha();
		ImGui::DragFloat("Alpha", &alpha, 0.01f, 0.0f, 1000.0f);
		kernel.setAlpha(alpha);
		ImGui::PopItemWidth();
	}
};
void
drawToneMappingAlgorithmsCombo(clRenderer::Renderer &renderer) {
	if (renderer.isRecompilationRequested() || !renderer.getCurrentDevice()) {
		ImGui::Text("No Tone-mapping");
		return;
	}
	ImGui::PushItemWidth(200);

	std::size_t selectedI = renderer.getSelectedToneMapperI();

	const auto &toneMappers = renderer.getToneMappers();
	std::size_t numToneMappers = toneMappers.size();
	const char *selectedName = toneMappers[selectedI]->getName().c_str();
	ImGui::Text("Tone-mapping:");
	ImGui::SameLine();
	if (ImGui::BeginCombo("###HDR Tone-mapping", selectedName)) {
		for (std::size_t i = 0; i < numToneMappers; ++i) {
			const auto &toneMapper = toneMappers[i];
			bool selected = i == selectedI;
			if (ImGui::Selectable(toneMapper->getName().c_str(), selected))
				selectedI = i;
		}
		ImGui::EndCombo();
	}
	ImGui::SameLine();
	toneMappers[selectedI]->acceptVisit(TMArgsDrawVisitor{});

	renderer.selectToneMapper(selectedI);
	ImGui::PopItemWidth();
}
} // namespace

// namespace
void
SceneView::draw(Program &program, const TwoPanelParams &parameters) {
	auto &renderer = program.getRenderer();
	ImGui::AlignFirstTextHeightToWidgets();
	ImGui::Text(renderStringFromStats(
					renderer.getTextureWidth(), renderer.getTextureHeight(),
					renderer.getNumSamples(), renderer.getMilisecondsElapsed())
					.c_str());
	drawToneMappingAlgorithmsCombo(renderer);
	ImGui::SameLine();
	imageSaving(program, parameters);

	ImGui::Separator();
	// Scale and position the image
	glm::vec2 cursorPos = ImGui::GetCursorScreenPos();
	auto sizeTaken = cursorPos - parameters.mainCanvasPos();
	auto availableSize = parameters.mainCanvasSize() - sizeTaken;
	auto imageSize = program.getTextureDims();
	float s = glm::compMin(availableSize / imageSize) - 0.05f;
	imageSize *= s;
	auto canvasCenter = availableSize / 2.0f;
	auto imageCenter = imageSize / 2.0f;
	auto cursorShift = canvasCenter - imageCenter;
	cursorPos += cursorShift;

	ImGui::SetCursorScreenPos(cursorPos);
	auto texID = reinterpret_cast<ImTextureID>(program.getTextureID());
	ImGui::Image(texID, imageSize);
	if (ImGui::IsItemHovered()) // Zoomed image
	{
		ImGui::BeginTooltip();
		glm::vec2 mousePos = ImGui::GetMousePos();
		mousePos -= cursorPos;
		mousePos /= imageSize;

		ImVec2 uv0 = glm::max(mousePos - 0.05f, 0.0f);
		ImVec2 uv1 = glm::max(mousePos + 0.05f, 0.0f);
		ImGui::Image(texID, imageSize / 2.0f, uv0, uv1);
		ImGui::EndTooltip();
	}
	trackballControl(scene->getCam(), renderer, cursorPos, imageSize);
}
void
SceneView::trackballControl(clRenderer::Camera &cam,
							clRenderer::Renderer &renderer,
							const glm::vec2 &imgPos, const glm::vec2 &imRes) {
	// User just started dragging
	if (!trackball.isDragging && ImGui::IsMouseDragging() &&
		ImGui::IsItemHovered()) {
		trackball.isDragging = true;
		// Cache the camera
		trackball.cachedCamera = cam;
		trackball.invView = cam.getInvView();
	}
	// User just stopped dragging
	else if (!ImGui::IsMouseDragging()) {
		trackball.isDragging = false;
	}

	if (trackball.isDragging) {

		glm::vec2 mouseDrag = ImGui::GetMouseDragDelta();
		glm::vec2 currMPos = ImGui::GetMousePos();
		glm::vec2 origMPos = currMPos - mouseDrag;
		// From pixel coords to [0,1] on the image.
		currMPos = 2.0f * (currMPos - imgPos) / imRes - glm::vec2(1.0f);
		origMPos = 2.0f * (origMPos - imgPos) / imRes - glm::vec2(1.0f);
		// Y is up.
		currMPos.y *= -1.0f;
		origMPos.y *= -1.0f;
		// Project vectors on the front of the trackball sphere.
		float z1 = std::sqrt(std::max(1.0f - glm::length(origMPos), 0.0f));
		float z2 = std::sqrt(std::max(1.0f - glm::length(currMPos), 0.0f));
		glm::vec3 spOrig{origMPos, z1};
		glm::vec3 spCurr{currMPos, z2};

		spOrig = glm::normalize(spOrig);
		spCurr = glm::normalize(spCurr);

		float cosAngle = glm::dot(spOrig, spCurr);
		auto axis = glm::normalize(glm::cross(spCurr, spOrig));
		auto worldAxis = glm::vec3(trackball.invView * glm::vec4(axis, 0.0f));
		// Set the camera to the one at the start of the dragging
		// and rotate it accordingly.
		const auto &c = trackball.cachedCamera;
		cam.lookAt(c.getPos(), c.getTargetPos(), c.getUpDir());
		cam.rotateAround(worldAxis, glm::acos(cosAngle));
		renderer.requestRecompilation(
			clRenderer::Renderer::RecompilationFlags::Camera);
	} else // Allow zooming with wheel
	{
		return;
		auto &io = ImGui::GetIO();
		if (glm::abs(io.MouseWheel) > 0.1f) {
			auto viewDir = cam.getDir();
			auto offset = viewDir * io.MouseWheel * 0.1f;

			auto newCamPos = viewDir + offset;
			// We didn't zoom top close to the target
			if (glm::dot(newCamPos - cam.getPos(),
						 cam.getTargetPos() - cam.getPos()) > 1.0f)
				cam.lookAt(newCamPos, cam.getTargetPos(), cam.getUpDir());
			renderer.requestRecompilation(
				clRenderer::Renderer::RecompilationFlags::Camera);
		}
	}
}

namespace {

// Creates and returns afilename for an image based on the current time and
// rendered samples.
std::string
genImageName(std::size_t renderedSamples) {
	// https://stackoverflow.com/questions/17223096/outputting-date-and-time-in-c-using-stdchrono
	auto now = std::chrono::system_clock::now();
	auto in_time_t = std::chrono::system_clock::to_time_t(now);
	std::stringstream ss;
	ss << "Image-" << renderedSamples << '-';
	ss << std::put_time(std::localtime(&in_time_t), "%F-%H-%M-%S");
	return ss.str();
}
} // namespace
void
SceneView::imageSaving(Program &program, const TwoPanelParams &parameters) {
	// Not currently saving && clicked
	if (!maybeSavedImage && ImGui::Button("Save Image")) {
		std::string name = genImageName(program.getRenderer().getNumSamples());
		maybeSavedImage = program.saveImage(name, false);
		parameters.getConsole().printLine("Saving " + name);
	} else if (maybeSavedImage) {
		ImGui::Text("Saving...");
		if (maybeSavedImage->wait_for(std::chrono::seconds(0)) ==
			std::future_status::ready) {
			if (maybeSavedImage->get())
				parameters.getConsole().printLine(
					"The image was successfuly saved.");
			else
				parameters.getConsole().printLine("Failed to save the image.");

			maybeSavedImage.reset();
		}
	}
}
} // namespace brdfEditor::gui::sceRen