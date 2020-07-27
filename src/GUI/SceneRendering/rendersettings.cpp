#include "RenderSettings.hpp"

#include <filesystem>
#include <functional>
#include <optional>

#include <ImGui/imgui.h>

#include <src/Core/Program.hpp>
#include <src/OpenCLRenderer/SceneLoader.hpp>
#include <src/GUI/CommonElements.hpp>

namespace brdfEditor {

using Platform = clRenderer::PlatformInfo;
using Platforms = std::vector<Platform>;
using Device = clRenderer::DeviceInfo;
using Devices = std::vector<Device>;
namespace {

// Colors representing support for OpenCL platforms and devices in
// RenderSettings window.
const ImVec4 okCol{0.11f, 0.71f, 0.03f, 1.0f};
// Limited support.
const ImVec4 limCol{0.98f, 0.46f, 0.01f, 1.0f};
const ImVec4 errCol{1.0f, 0.0f, 0.0f, 1.0f};

const char *sceneLoadPopupStr = "Load New Scene##Popup";
const char *resolutionPopupStr = "Change texture dimensions##Popup";
const char *newKernelPopup = "Select a new kernel##Popup";

// Draws a selectable with colored label and tooltip on hover.
// --TooltipFnc: No parameters, return type is ignored.
// Returns: Whether the selectable was clicked.
template <typename TooltipFnc>
bool
colHovSelectable(const char *label, TooltipFnc &&tooltipFnc, ImVec4 col,
				 bool selected) {
	auto &&cPos = ImGui::GetCursorPos();
	ImGui::PushID(label);
	bool ret = ImGui::Selectable("##selectable", selected);
	if (ImGui::IsItemHovered())
		tooltipFnc();
	ImGui::PopID();
	ImGui::SetCursorPos(cPos);
	ImGui::TextColored(col, label);
	return ret;
}

// Tooltip drawn when hovering over a platform in platforms' combo.
void
drawComboPlatformTooltip(const Platform &platform) {
	ImGui::BeginTooltip();
	if (platform.versionOK)
		ImGui::TextColored(okCol, platform.clVersion.c_str());
	else
		ImGui::TextColored(errCol,
						   (platform.clVersion + " => CANNOT SELECT").c_str());
	if (platform.interopAvail)
		ImGui::TextColored(okCol, "Interop available");
	else
		ImGui::TextColored(limCol, "Interop not avaialble. Buffered approach "
								   "will be used.");
	ImGui::EndTooltip();
}
// Allows choosing a platform from the available platforms.
// If a different platform is selected then currPlatform is updated and resets
// renderer's device to nullptr. Might also set currPlatform to nullptr
void
drawSettingsPlatformsCombo(const Platform *&currPlatform,
						   const Platforms &platforms,
						   clRenderer::Renderer &renderer) {
	const char *currPlatName =
		currPlatform ? currPlatform->name.c_str() : "Please select a platform";
	if (ImGui::BeginCombo("Platforms", currPlatName)) {
		for (auto &&p : platforms) {

			bool selected = currPlatform
								? currPlatform->clPlatform() == p.clPlatform()
								: false;
			const ImVec4 &col =
				p.versionOK ? (p.interopAvail ? okCol : limCol) : errCol;
			ImGui::PushID(p.clPlatform());
			auto &&tooltipFnc = [&p = p]() { drawComboPlatformTooltip(p); };
			// Only select non-selected platforms
			if (colHovSelectable(p.name.c_str(), tooltipFnc, col, selected) &&
				!selected) {
				// Only select supported platforms
				currPlatform = p.versionOK ? &p : nullptr;
				// Reset the device
				renderer.setDevice(nullptr);
			}
			ImGui::PopID();
		}
		ImGui::EndCombo();
	}
}
// The tooltip drawn when hovering over a device in devices' combo.
void
drawComboDeviceTooltip(const Device &dev) {
	ImGui::BeginTooltip();
	ImVec4 color;
	const char *text;
	if (dev.versionOK) {
		color = okCol;
		text = dev.clVersion.c_str();
	} else {
		color = errCol;
		text = (dev.clVersion + " => CANNOT SELECT").c_str();
	}
	ImGui::TextColored(color, text);
	ImGui::TextColored(color, dev.type == Device::GPU ? "GPU" : "CPU");
	if (dev.interopOK)
		ImGui::TextColored(okCol, "Interop available");
	else
		ImGui::TextColored(limCol, "Interop not avaialble. Buffered approach "
								   "will be used.");
	ImGui::EndTooltip();
}
// Allows choosing a device from available devices.
// If a different device is selected then sets it in renderer an updates
// currDevice. Might set currDevice to nullptr
void
drawSettingsDevicesCombo(const Platform &currPlatform,
						 const Device *&currDevice,
						 clRenderer::Renderer &renderer) {
	const char *currDeviceName =
		currDevice ? currDevice->name.c_str() : "Please select a device";
	if (ImGui::BeginCombo("Devices", currDeviceName)) {
		for (auto &&d : currPlatform.devices) {

			bool selected =
				currDevice ? currDevice->clDevice() == d.clDevice() : false;
			const ImVec4 &col =
				d.versionOK ? (d.interopOK ? okCol : limCol) : errCol;
			auto &&tooltipFnc = [&d = d]() { drawComboDeviceTooltip(d); };
			// Only select non-selected devices
			if (colHovSelectable(d.name.c_str(), tooltipFnc, col, selected) &&
				!selected)
				// Only select supported devices
				renderer.setDevice(currDevice = d.versionOK ? &d : nullptr);
		}
		ImGui::EndCombo();
	}
}
// Colored and centered button that controls the rendering.
void
StartPauseRenderingButton(Program &program) {
	const auto &renderer = program.getRenderer();
	const char *text;
	ImVec4 color;
	std::function<void(void)> action;
	if (!program.isRendering()) {
		if (renderer.getNumSamples() == 0) {
			text = "Start Rendering";
			color = okCol;
			action = [&p = program]() { p.resumeRendering(); };
		} else if (renderer.getNumSamples() == program.maxSamplesToRender()) {
			text = "Finished, restart?";
			color = okCol;
			action = [&p = program]() {
				p.getRenderer().requestRecompilation(
					clRenderer::Renderer::RecompilationFlags::MaterialsParameters);
				p.resumeRendering();
			};
		} else {
			text = "Resume Rendering";
			color = limCol;
			action = [&p = program]() { p.resumeRendering(); };
		}
	} else {
		text = "Pause Rendering";
		color = errCol;
		action = [&p = program]() { p.pauseRendering(); };
	}

	auto buttonWidth = ImGui::CalcTextSize(text).x;
	auto indentAmount =
		(ImGui::GetContentRegionAvailWidth() - buttonWidth) / 2.0f;
	ImGui::SetCursorPosX(indentAmount);
	ImGui::PushStyleColor(ImGuiCol_Button, color);
	if (ImGui::Button(text))
		action();
	ImGui::PopStyleColor();
}
} // namespace

namespace gui::sceRen {
RenderSettings::RenderSettings(SceneEvents &sceneEvents, Console &console) :
	sceneEvents(&sceneEvents),
	console(&console),
	sceneLoadPopupOpen(false),
	resolutionPopupOpen(false),
	newKernelPopupOpen(false) {}

void
RenderSettings::draw(Program &program) {
	auto &renderer = program.getRenderer();

	// Pause rendering for popups
	if (sceneLoadPopupOpen) {
		ImGui::OpenPopup(sceneLoadPopupStr);
		program.pauseRendering();
	}
	if (resolutionPopupOpen) {
		ImGui::OpenPopup(resolutionPopupStr);
		program.pauseRendering();
	}
	if (newKernelPopupOpen) {
		ImGui::OpenPopup(newKernelPopup);
		program.pauseRendering();
	}
	sceneLoadPopup(program.getScene(), renderer);
	setResolutionPopup(program);
	kernelSelectPopup(renderer);
	if (!ImGui::CollapsingHeader("Render Settings",
								 ImGuiTreeNodeFlags_DefaultOpen))
		return;

	const Platforms &platforms = renderer.getAvailablePlatforms();
	const Device *currDevice = renderer.getCurrentDevice();
	// If a device is selected, then the renderer is fully working, so fetch
	// current platform. Otherwise use temporary platform.
	currPlatform = currDevice ? currDevice->platform : currPlatform;

	drawSettingsPlatformsCombo(currPlatform, platforms, renderer);
	// Fetch the device again because a change in the platform might change the
	// current device.
	currDevice = renderer.getCurrentDevice();
	if (currPlatform)
		drawSettingsDevicesCombo(*currPlatform, currDevice, renderer);
	if (!currDevice)
		return;
	int maxSamples = static_cast<int>(program.maxSamplesToRender());
	ImGui::InputInt("Max samples", &maxSamples);
	program.maxSamplesToRender() =
		static_cast<std::size_t>(std::max(maxSamples, 1));
	// TODO Implement
	// Use a modals for all of them.
	if (ImGui::Button("Set Image Resolution"))
		resolutionPopupOpen = true;
	ImGui::SameLine();
	if (ImGui::Button("Set Scene"))
		sceneLoadPopupOpen = true;
	ImGui::SameLine();
	if (ImGui::Button("Set kernel"))
		newKernelPopupOpen = true;

	int maxBounces = (int)renderer.getMaxBounces();
	ImGui::InputInt("Max # bounces", &maxBounces);
	renderer.setMaxBounces((int)std::max(1, maxBounces));

	if (currDevice)
		StartPauseRenderingButton(program);
}

void
RenderSettings::sceneLoadPopup(clRenderer::Scene &scene,
							   clRenderer::Renderer &renderer) {
	using namespace clRenderer;
	namespace fs = std::filesystem;
	if (ImGui::BeginPopupModal(sceneLoadPopupStr, &sceneLoadPopupOpen,
							   ImGuiWindowFlags_AlwaysAutoResize)) {

		static std::optional<std::string> newFilename;

		bool close = false;

		drawInputFilenameCombo("./Scenes", newFilename);

		ImGui::TextColored(errCol,
						   "WARNING: All materials, BRDFs and graphs will be \n"
						   "cleared and replaced with the new scene.\n The "
						   "loading may take a while because the environment \n"
						   "map sampling and BVH structures must be built.");
		if (newFilename && ImGui::Button("Load")) {
			try {
				Scene newScene = loadSceneFromFile(*newFilename);
				scene = std::move(newScene);
				sceneEvents->raiseSceneReloadedEvent();
				renderer.requestRecompilation(
					Renderer::RecompilationFlags::SceneObjects |
					Renderer::RecompilationFlags::BRDFs);
				close = true;
				console->printLine("Successfuly loaded a new scene from '" +
								   std::string(*newFilename) + "'.");
			} catch (const SceneFormatException &e) {
				close = true;
				console->printLine(
					std::string("Scene could not be loaded because: \n\t") +
					e.what());
			}
		}
		if (newFilename)
			ImGui::SameLine();
		if (ImGui::Button("Cancel")) {
			close = true;
		}
		if (close) {
			sceneLoadPopupOpen = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}
namespace {}
void
RenderSettings::kernelSelectPopup(clRenderer::Renderer &renderer) {
	using namespace clRenderer;
	namespace fs = std::filesystem;
	if (ImGui::BeginPopupModal(newKernelPopup, &newKernelPopupOpen,
							   ImGuiWindowFlags_AlwaysAutoResize)) {

		static std::optional<std::string> newFilename;

		bool close = false;

		drawInputFilenameCombo("./Kernels", newFilename);

		if (newFilename && ImGui::Button("Load")) {
			renderer.switchKernel(newFilename->c_str());
			close = true;
			console->printLine("Selected new kernel, it will be compiled when "
							   "the rendering starts.'");
		}
		if (newFilename)
			ImGui::SameLine();
		if (ImGui::Button("Cancel")) {
			close = true;
		}
		if (close) {
			newKernelPopupOpen = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}

void
RenderSettings::setResolutionPopup(Program &program) {
	using namespace clRenderer;
	if (ImGui::BeginPopupModal(resolutionPopupStr, &resolutionPopupOpen,
							   ImGuiWindowFlags_AlwaysAutoResize)) {

		static int width = 0, height = 0;

		bool close = false;
		ImGui::InputInt("Width", &width);
		ImGui::InputInt("Height", &height);
		if (ImGui::Button("Set")) {
			program.setTextureDims(width, height);
			close = true;
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel")) {
			close = true;
		}
		if (close) {
			resolutionPopupOpen = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}

} // namespace gui::sceRen
} // namespace brdfEditor
