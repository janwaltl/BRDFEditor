#include "EditorParams.hpp"

#include <algorithm>

#include <src/OpenCLRenderer/Scene.hpp>

namespace brdfEditor::gui::editor {

EditorParams::EditorParams(clRenderer::Scene &scene, SceneEvents &sceneEvents,
						   Console &console) :
	scene(&scene),
	sceneEvents(&sceneEvents),
	console(&console) {

	auto reloadSources = [this]() { this->getEditedBRDFs().clear(); };
	sceneEvents.subscribeToSceneReloading(reloadSources);

	reloadSources();
}
std::vector<BRDFEntry> &
EditorParams::getEditedBRDFs() {
	return editedBRDFs;
}
clRenderer::Scene &
EditorParams::getScene() const {
	return *scene;
}
Console &
EditorParams::getConsole() const {
	return *console;
}
void
EditorParams::selectBRDF(const std::string *name) {
	if (name != nullptr)
		selectedBRDF = *name;
	else
		selectedBRDF.reset();
}
const std::optional<std::string> &
EditorParams::getSelectedBRDF() const {
	return selectedBRDF;
}
void
EditorParams::editBRDF(const std::string &name) {
	auto IT = std::find_if(editedBRDFs.begin(), editedBRDFs.end(),
						   [&name = name](const BRDFEntry &entry) {
							   return entry.name == name;
						   });
	// If the BRDF has been already edited, just open its tab.
	if (IT != editedBRDFs.end())
		IT->opened = true;
	else {
		// Search for the source and create new BRDFEntry.
		auto maybeBRDF = scene->getBRDF(name);
		// Should be found as valid name is functions precondition
		assert(maybeBRDF);
		auto &&[brdfName, brdf] = **maybeBRDF;
		assert(brdfName == name);
		const std::string &source = brdf.getSource();

		BRDFEntry entry;
		entry.opened = true;
		entry.name = brdfName;
		std::copy(source.cbegin(), source.cend(),
				  std::back_inserter(entry.source));
		entry.source.push_back('\0');
		editedBRDFs.push_back(std::move(entry));
	}
}
bool
EditorParams::removeMaterial(const std::string &name) {
	try {
		bool removed = scene->removeMaterial(name);
		sceneEvents->raiseMatRemovalEvent(name);
		if (!removed)
			return false;
		console->printLine("Material '" + name + "' was removed.");
		return true;
	} catch (const clRenderer::InUseSceneException &e) {
		console->printLine(e.what());
		return false;
	}
}
bool
EditorParams::removeBRDF(const std::string &name) {
	if (scene->isBRDFUsed(name)) {
		console->printLine(
			"A material associated with BRDF '" + name +
			"' is used by some object in the Scene. BRDF was not removed but "
			"some of its materials might have been.");
		return false;
	}
	 
	auto &&maybeBRDFHandle = scene->getBRDF(name);
	if (!maybeBRDFHandle)
		return false;
	auto &&[brdfName2, brdf] = **maybeBRDFHandle;
	try {
		// Should succeed as the BRDF is not in use so raise events.
		for (auto &&associatedMat : brdf.materials)
			sceneEvents->raiseMatRemovalEvent(associatedMat);

		bool removed = scene->removeBRDF(name);
		assert(removed);
		if (removed) {
			console->printLine("BRDF '" + name + "' was removed.");

			// Delete sources and BRDFEntry if there is one.
			auto IT = std::find_if(editedBRDFs.begin(), editedBRDFs.end(),
								   [&name](const BRDFEntry &entry) {
									   return entry.name == name;
								   });
			if (IT != editedBRDFs.end())
				editedBRDFs.erase(IT);

			// Reset selection if this BRDF was the selected one
			if (selectedBRDF && *selectedBRDF == name)
				selectedBRDF.reset();
			// Reset selection if material no longer exists
			if (selectedMaterial && !scene->getMaterial(*selectedMaterial))
				selectedMaterial.reset();
		}
		return removed;
	} catch (const clRenderer::InUseSceneException &e) {
		console->printLine(e.what());
		return false;
	}
}
namespace {
// Updates parameters of the materials associated with the passed BRDF.
// Deletes any parameters no longer present in the BRDF, adds news parameters
// and updates others if they change. If a parameter did not changed then its
// value in all materials remains unchanged, not resetted to the default if the
// default changed.
void
fixMaterialsParameters(const clRenderer::BRDF &brdf, clRenderer::Scene &scene) {
	// Update parameters of all associated materials
	for (auto &&matName : brdf.materials) {
		auto &&maybeMatHandle = scene.getMaterial(matName);
		assert(maybeMatHandle);
		auto &&[matName2, material] = **maybeMatHandle;
		// Extract old parameters and replace them with the new ones
		auto &&oldMatParams = std::move(material.params);
		material.params.clear();
		material.params = brdf.parameters;
		// Copy any old parameters'values if those parameters are still present.
		for (auto &&[paramName, param] : material.params) {
			auto &&oldParamIT = oldMatParams.find(paramName);
			if (oldParamIT != oldMatParams.end() &&
				oldParamIT->second.isSameTypeAs(param))
				param.visit([oldParamIT](auto &&p) {
					using paramType = std::decay_t<decltype(p)>;
					// Safe to call get because they have same types.
					auto &&oldParamValue = oldParamIT->second.get<paramType>()
											   .value;
					// Restore the parameter's value
					p.value = oldParamValue;
				});
		}
	}
}
} // namespace
bool
EditorParams::updateSources() {
	bool anyChanged = false;
	for (auto &&entry : editedBRDFs) {
		auto &&maybeBRDFHandle = scene->getBRDF(entry.name);
		
		auto &&[brdfName, brdf] = **maybeBRDFHandle;

		std::string newSource;
		// Do not copy '\0' from the vector
		newSource.resize(entry.source.size() - 1);
		for (std::size_t i = 0; i < newSource.size(); ++i)
			newSource[i] = entry.source[i];
		if (brdf.getSource() != newSource) {
			anyChanged = true;
			try {
				brdf.changeSource(newSource);
				fixMaterialsParameters(brdf, *scene);
				sceneEvents->raiseBRDFSourceChanged(brdfName);
			} catch (const clRenderer::BRDF::FormatException &e) {
				std::string msg = "Encountered an error when parsing ";
				msg += brdfName + ":\n";
				msg += e.what();
				console->printLine(msg);
			}
			catch (const Exception& e) {// Catch recompilation errors.
				console->printLine(std::string("Graph Kernel Error:") + e.what());
			}
			console->printLine(
				"Due to a change to a BRDF's source some materials "
				"might have been changed.");
		}
	}
	return anyChanged;
}
void
EditorParams::selectMaterial(const std::string *material) {
	if (material)
		selectedMaterial = *material;
	else
		selectedMaterial.reset();
}
const std::optional<std::string> &
EditorParams::getSelectedMaterial() const {
	return selectedMaterial;
}
} // namespace brdfEditor::gui::editor
