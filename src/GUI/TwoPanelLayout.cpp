#include "TwoPanelLayout.hpp"

#include <ImGui/imgui.h>

namespace brdfEditor::gui {

namespace {
// Pixel spacing between the panels.
constexpr const float winSeparation = 5.0f;
// Relative width of the console compared to the main panel
constexpr const float relSpaceForConsole = 0.3f;
} // namespace

Console &
TwoPanelParams::getConsole() const {
	return *console;
}
bool
TwoPanelParams::isConsoleOpen() const {
	return consoleOpened;
}
void
TwoPanelParams::switchConsole(bool open) {
	consoleOpened = open;
}
float
TwoPanelParams::dividerPos() const {
	return divider;
}
const glm::vec2 &
TwoPanelParams::leftCanvasPos() const {
	return leftCanvas.position;
}
const glm::vec2 &
TwoPanelParams::leftCanvasSize() const {
	return leftCanvas.size;
}
const glm::vec2 &
TwoPanelParams::mainCanvasPos() const {
	return mainCanvas.position;
}
const glm::vec2 &
TwoPanelParams::mainCanvasSize() const {
	return mainCanvas.size;
}
const std::string &
TwoPanelParams::leftTitle() const {
	return leftCanvas.title;
}
std::string &
TwoPanelParams::leftTitle() {
	return leftCanvas.title;
}
const std::string &
TwoPanelParams::mainTitle() const {
	return mainCanvas.title;
}
std::string &
TwoPanelParams::mainTitle() {
	return mainCanvas.title;
}

const glm::vec2 &
TwoPanelParams::consoleCanvasPos() const {
	return consolePos;
}

const glm::vec2 &
TwoPanelParams::consoleCanvasSize() const {
	return consoleSize;
}

TwoPanelLayout::TwoPanelLayout(Console &console, GUIParams &parameters) :
	TwoPanelLayout(console, parameters, "Tools", "Main") {}
TwoPanelLayout::TwoPanelLayout(Console &console, GUIParams &parameters,
							   std::string leftTitle, std::string rightTitle) :
	guiParams(&parameters) {
	panelParams.console = &console;
	panelParams.consoleOpened = true;
	panelParams.divider = 0.3f;
	panelParams.leftCanvas.title = std::move(leftTitle);
	panelParams.mainCanvas.title = std::move(rightTitle);

	updateParameters();
}
namespace {

// --postfix: a string attached to the title if it's not empty. Used for ImGui
// window IDs.
// Return: Title and flags of the windows based on input parameters.
std::pair<std::string, ImGuiWindowFlags>
genWindowOptions(const std::string &title, const char *postfix) {
	auto flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
				 ImGuiWindowFlags_NoBringToFrontOnFocus |
				 ImGuiWindowFlags_NoSavedSettings |
				 ImGuiWindowFlags_AlwaysUseWindowPadding;
	if (title.empty())
		return {postfix, flags | ImGuiWindowFlags_NoTitleBar};
	else
		return {title + "##" + postfix, flags};
}
} // namespace
void
TwoPanelLayout::draw() {
	updateParameters();
	panelParams.console->clearNewOutputFlag();

	ImGui::SetNextWindowPos(panelParams.leftCanvasPos(), ImGuiCond_Always);
	ImGui::SetNextWindowSize(panelParams.leftCanvasSize(), ImGuiCond_Always);

	auto &&[title, flags] =
		genWindowOptions(panelParams.leftTitle(), "LeftPanel");
	if (ImGui::Begin(title.c_str(), nullptr, flags))
		leftPanelContent();
	ImGui::End();

	updateMainPanelParameters();

	ImGui::SetNextWindowPos(panelParams.mainCanvasPos(), ImGuiCond_Always);
	ImGui::SetNextWindowSize(panelParams.mainCanvasSize(), ImGuiCond_Always);

	std::tie(title, flags) =
		genWindowOptions(panelParams.mainTitle(), "MainPanel");
	if (ImGui::Begin(title.c_str(), nullptr, flags))
		mainPanelContent();
	ImGui::End();

	ImGui::SetNextWindowPos(panelParams.consoleCanvasPos(), ImGuiCond_Always);
	ImGui::SetNextWindowSize(panelParams.consoleCanvasSize(), ImGuiCond_Always);

	std::tie(title, flags) = genWindowOptions("Console", "Console");
	// Open the console if there's something new
	if (panelParams.console->newOutputThisFrame())
		ImGui::SetNextWindowCollapsed(false);
	if (ImGui::Begin(title.c_str(), nullptr, flags)) {
		// If the window uncollapsed, set the console as opened.
		panelParams.consoleOpened = true;
		drawConsole();
	} else
		panelParams.consoleOpened = false;
	ImGui::End();
}

const TwoPanelParams &
TwoPanelLayout::parameters() {
	return panelParams;
}

void
TwoPanelLayout::updateParameters() {
	panelParams.leftCanvas.position = guiParams->canvasPos();
	panelParams.leftCanvas.size = guiParams->canvasSize();
	panelParams.leftCanvas.size.x *= panelParams.divider;

	updateMainPanelParameters();
}

void
TwoPanelLayout::updateMainPanelParameters() {
	panelParams.mainCanvas.position = guiParams->canvasPos();
	panelParams.mainCanvas.position.x +=
		panelParams.leftCanvas.size.x + winSeparation;
	panelParams.mainCanvas.size = guiParams->canvasSize();
	panelParams.mainCanvas.size.x *= 1.0f - panelParams.divider;
	panelParams.mainCanvas.size.x -= winSeparation;
	float consoleSpace =
		panelParams.isConsoleOpen()
			? relSpaceForConsole
			: (ImGui::GetFrameHeightWithSpacing() / guiParams->resolution().y);

	panelParams.mainCanvas.size.y *= 1.0f - consoleSpace;
	panelParams.consolePos = panelParams.mainCanvasPos();
	panelParams.consolePos.y += panelParams.mainCanvasSize().y + winSeparation;
	panelParams.consoleSize =
		guiParams->canvasSize() *
		glm::vec2(1.0f - panelParams.divider, consoleSpace);

	panelParams.consoleSize -= glm::vec2{winSeparation, winSeparation};
}
void
TwoPanelLayout::drawConsole() {
	// TODO Replace with proper console implementation.
	ImGui::TextUnformatted(panelParams.console->getBuffer());
	if (panelParams.console->newOutputThisFrame()) {
		ImGui::SetScrollHereY(1.0f);
	}
}
} // namespace brdfEditor::gui