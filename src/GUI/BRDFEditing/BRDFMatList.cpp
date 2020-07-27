#include "BRDFMatList.hpp"

#include <array>
#include <fstream>
#include <sstream>

#include <ImGui/imgui.h>

#include <src/OpenCLRenderer/Renderer/Renderer.hpp>
#include <src/OpenCLRenderer/Scene.hpp>
#include <src/GUI/CommonElements.hpp>

namespace brdfEditor::gui::editor {
namespace {

constexpr const char *noDeviceMsg =
	"Kernel can only be compiled if a rendering "
	"device is selected. Please select a rendering "
	"platform and the device in SceneRendering tab.";
constexpr const char *addMaterialPopupStr = "Add Material##Popup";
constexpr const char *newBRDFPopupStr = "New BRDF##Popup";
// Draws "Compile kernel" button which if pressed updates all edited BRDFs with
// their new sources and then recompiles the kernel.
// Results are printed into the console.
void
RecompileKernelWithButton(EditorParams &params, Console &console,
						  clRenderer::Renderer &renderer) {
	// Save results to avoid too long if/else statements
	bool deviceSet = renderer.getCurrentDevice() != nullptr;
	bool buttonPressed = ImGui::Button("Compile Kernel");
	bool buttonHovered = ImGui::IsItemHovered();

	// Short-circuit
	bool sourceChanged = buttonPressed && params.updateSources();
	try {
		if (buttonPressed && !deviceSet) {
			console.printLine(noDeviceMsg);
		} else if (buttonPressed &&
				   (sourceChanged || renderer.isRecompilationRequested())) {
			renderer.requestRecompilation(
				clRenderer::Renderer::RecompilationFlags::BRDFs);
			renderer.recompile();
			console.printLine("Kernel recompiled successfuly.");
		} else if (buttonPressed) {
			console.printLine(
				"No source has been changed. Kernel not modified.");
		} else if (!deviceSet && buttonHovered) {
			ImGui::SetTooltip(noDeviceMsg);
		}
	} catch (const clRenderer::BRDF::FormatException &e) {
		console.printLine(e.what());
	} catch (const Exception &e) {
		console.printLine(e.what());
	}
}

// Indents the text to the right and leaves enough space for 'numButton' that
// together say 'textOnButtons'.
void
alignButtonsRight(std::size_t numButtons, const char *textOnButtons) {
	// Padding of the inner text of spacing between buttons
	const float buttonSpacing =
		ImGui::GetStyle().ItemSpacing.x + ImGui::GetStyle().FramePadding.x * 2;

	float offsetButtons = ImGui::GetContentRegionAvailWidth() -
						  ImGui::CalcTextSize(textOnButtons).x -
						  numButtons * buttonSpacing;
	ImGui::SameLine(offsetButtons);
}

// Caller should call ImGui::UnIndent later at some point.
void
indentButtonsCenter(std::size_t numButtons, const char *textOnButtons) {
	// Padding of the inner text of spacing between buttons
	const float buttonSpacing =
		ImGui::GetStyle().ItemSpacing.x + ImGui::GetStyle().FramePadding.x * 2;

	float offsetButtons = ImGui::GetContentRegionAvailWidth() -
						  ImGui::CalcTextSize(textOnButtons).x -
						  numButtons * buttonSpacing;
	ImGui::Indent(offsetButtons / 2.0f);
}

} // namespace

BRDFMatList::BRDFMatList(clRenderer::Renderer &renderer, EditorParams &params) :
	params(&params),
	renderer(&renderer) {
	addMatPopupData.justOpened = false;
	addMatPopupData.matName[0] = '\0';
	addMatPopupData.nameExists = false;
	addMatPopupData.open = false;

	newBRDFPopupData.justOpened = false;
	newBRDFPopupData.name[0] = '\0';
	newBRDFPopupData.nameExists = false;
	newBRDFPopupData.open = false;
	newBRDFPopupData.pasteFromClipboardCheck = false;
}

void
BRDFMatList::draw() {

	drawPopups();
	ImGui::BeginGroup();
	if (!ImGui::CollapsingHeader("BRDFunctions and Materials",
								 ImGuiTreeNodeFlags_DefaultOpen))
		return;

	if (ImGui::BeginChild("BRDFS", ImVec2(0, 300), true)) {
		auto &scene = params->getScene();
		// Used to distinguish individual ImGui elements
		int uniqueID = 0;
		// Draws list of BRDFs and to each draws another list with its
		// materials.
		drawBRDFs(scene, uniqueID);
		ImGui::Separator();
	}
	ImGui::EndChild();

	indentButtonsCenter(3, "Save Opened BRDF New BRDFCompile Kernel");
	if (ImGui::Button("New BRDF")) {
		newBRDFPopupData.open = true;
		newBRDFPopupData.justOpened = true;
	}
	ImGui::SameLine();
	if (params->getSelectedBRDF() && ImGui::Button("Save Opened BRDF")) {

		auto &editedBRDFs = params->getEditedBRDFs();
		auto IT = std::find_if(
			editedBRDFs.begin(), editedBRDFs.end(),
			[&](BRDFEntry &e) { return e.name == *params->getSelectedBRDF(); });
		assert(IT != editedBRDFs.end());

		auto &brdfEntry = *IT;
		std::ofstream out{brdfEntry.name + ".cl",
						  std::ios::out | std::ios::trunc};
		if (!out.is_open())
			params->getConsole().printLine("Cannot write to: '" +
										   brdfEntry.name +
										   ".cl'. The BRDF was not saved.");
		else {
			out << brdfEntry.name;
			params->getConsole().printLine("Saved to '" + brdfEntry.name + ".cl'.");
		}
	}
	ImGui::SameLine();

	RecompileKernelWithButton(*params, params->getConsole(), *renderer);
	ImGui::Unindent();
	ImGui::EndGroup();
} // namespace brdfEditor::gui::editor
namespace {}
void
BRDFMatList::drawBRDFs(clRenderer::Scene &scene, int &uniqueID) {
	for (auto &&brdfHandle : scene.getBRDFs()) {
		auto &&[brdfName, brdf] = *brdfHandle;
		ImGui::PushID(uniqueID++);
		ImGui::AlignFirstTextHeightToWidgets();
		ImGui::Text(brdfName.c_str());

		alignButtonsRight(3, "EditRemove");

		if (ImGui::Button("Edit"))
			params->editBRDF(brdfName);
		ImGui::SameLine();
		if (ImGui::Button("Remove")) {
			// Must copy the argument, names will be deleted
			if (params->removeBRDF(std::string(brdfName))) {
				// Don't print the materials and rest of BRDFs as 'brdf' and
				// 'scene.getBRDFs()' have been invalidated.
				ImGui::PopID();
				break;
			}
		}
		ImGui::Separator();
		drawMaterials(brdf, uniqueID, brdfName);
		ImGui::Separator();
		ImGui::PopID();
	}
}
void
BRDFMatList::drawMaterials(clRenderer::BRDF &brdf, int &uniqueID,
						   const std::string &brdfName) {
	for (auto &&matName : brdf.materials) {
		ImGui::PushID(uniqueID++);
		ImGui::AlignFirstTextHeightToWidgets();
		ImGui::Bullet();
		ImGui::Text(matName.c_str());
		alignButtonsRight(3, "EditRemove");

		if (ImGui::Button("Edit"))
			params->selectMaterial(&matName);
		ImGui::SameLine();

		if (ImGui::Button("Remove")) {
			// Must copy the argument, names will be deleted

			if (params->removeMaterial(std::string(matName))) {
				// Skip the rest of materials the iterators have been
				// invalidated.
				ImGui::PopID();
				break;
			}
		}
		ImGui::PopID();
	}
	ImGui::AlignFirstTextHeightToWidgets();
	ImGui::Bullet();
	if (ImGui::Button("Add Material")) {
		addMatPopupData.open = true;
		addMatPopupData.justOpened = true;
		addMatPopupData.brdfName = brdfName;
	}
}
void
BRDFMatList::drawPopups() {
	if (addMatPopupData.open) {
		ImGui::OpenPopup(addMaterialPopupStr);
		addMaterialPopup();
	}

	if (newBRDFPopupData.open) {
		ImGui::OpenPopup(newBRDFPopupStr);
		newBRDFPopup();
	}
}
void
BRDFMatList::addMaterialPopup() {

	if (ImGui::BeginPopupModal(addMaterialPopupStr, &addMatPopupData.open,
							   ImGuiWindowFlags_AlwaysAutoResize)) {

		bool close = false;

		if (addMatPopupData.justOpened ||
			ImGui::InputText("Name", addMatPopupData.matName,
							 addMatPopupData.matNameMaxLen))
			addMatPopupData.nameExists =
				this->params->getScene()
					.getMaterial(addMatPopupData.matName)
					.has_value();

		if (addMatPopupData.nameExists)
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f),
							   "Name is already in use.");
		if (!addMatPopupData.nameExists && ImGui::Button("Add")) {
			auto &scene = this->params->getScene();
			auto &&brdfHandle = scene.getBRDF(addMatPopupData.brdfName);
			// Selected BRDF must exists( = not have been removed)
			assert(brdfHandle);

			if (scene.createMaterial(addMatPopupData.matName, *brdfHandle))
				params->getConsole().printLine(
					"Material " + std::string(addMatPopupData.matName) +
					" created.");
			else
				params->getConsole().printLine(
					"Material could not have been created.");
			close = true;
		}
		if (!addMatPopupData.nameExists)
			ImGui::SameLine();
		if (ImGui::Button("Cancel"))
			close = true;

		if (close) {
			ImGui::CloseCurrentPopup();
			addMatPopupData.open = false;
		}

		addMatPopupData.justOpened = false;
		ImGui::EndPopup();
	}
}

namespace {

// Returns: Empty string if there's no filename. Otherwise the content of the
// file is returned.
// Throws: 'Exception' if a file could not been opened.
std::string
loadBRDFSource(const std::optional<std::string> &filename) {
	if (!filename)
		return "";
	std::ifstream input{*filename};

	if (!input.is_open())
		throw Exception("File " + *filename + " could not be opened.");
	std::stringstream buffer;
	buffer << input.rdbuf();

	return buffer.str();
}

void
createBRDF(const std::optional<std::string> &filename,
		   const char *clipboardText, bool useClipboard, const char *name,
		   clRenderer::Scene &scene) {

	std::string source =
		useClipboard ? clipboardText : loadBRDFSource(filename);

	bool created = scene.createBRDF(name, std::move(source));
	assert(created);
}

} // namespace

void
BRDFMatList::newBRDFPopup() {
	if (ImGui::BeginPopupModal(newBRDFPopupStr, &newBRDFPopupData.open,
							   ImGuiWindowFlags_AlwaysAutoResize)) {

		bool close = false;
		auto &scene = this->params->getScene();

		ImGui::PushItemWidth(200);
		if (newBRDFPopupData.justOpened ||
			ImGui::InputText("Name", newBRDFPopupData.name,
							 newBRDFPopupData.nameMaxLen))
			newBRDFPopupData.nameExists =
				scene.getBRDF(newBRDFPopupData.name).has_value();

		if (newBRDFPopupData.nameExists)
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f),
							   "Name is already in use.");

		ImGui::Text("Source file for the new BRDF:");
		if (!newBRDFPopupData.pasteFromClipboardCheck)
			drawInputFilenameCombo("./BRDF", newBRDFPopupData.selectedBRDFFile);

		ImGui::PopItemWidth();

		const char *clipBoardText = ImGui::GetClipboardText();
		auto clipTextLen = clipBoardText ? std::strlen(clipBoardText) : 0;

		ImGui::Text("Use clipboard contents instead (contains %ll chars):",
					clipTextLen);
		ImGui::SameLine();
		ImGui::Checkbox("##useClipboard",
						&newBRDFPopupData.pasteFromClipboardCheck);

		if (!newBRDFPopupData.nameExists && ImGui::Button("Create")) {
			auto &console = params->getConsole();
			try {
				std::string name{newBRDFPopupData.name};
				createBRDF(newBRDFPopupData.selectedBRDFFile, clipBoardText,
						   newBRDFPopupData.pasteFromClipboardCheck,
						   name.c_str(), scene);

				console.printLine("BRDF " + name + " successfuly created.");
				params->editBRDF(name);
				renderer->requestRecompilation(
					clRenderer::Renderer::RecompilationFlags::BRDFs);
			} catch (const Exception &e) {
				console.printLine("BRDF could not be created, reason: " +
								  std::string(e.what()));
			}
			close = true;
		}
		if (!newBRDFPopupData.nameExists)
			ImGui::SameLine();
		if (ImGui::Button("Cancel"))
			close = true;

		if (close) {
			ImGui::CloseCurrentPopup();
			newBRDFPopupData.open = false;
		}
		newBRDFPopupData.justOpened = false;
		ImGui::EndPopup();
	}
}
} // namespace brdfEditor::gui::editor