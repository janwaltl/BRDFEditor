#ifndef BRDFEDITOR_GUI_EDITING_EDITOR_PARAMS_HEADER
#define BRDFEDITOR_GUI_EDITING_EDITOR_PARAMS_HEADER

#include <map>
#include <optional>
#include <string>
#include <vector>

#include <src/OpenCLRenderer/Materials/Material.hpp>
#include <src/GUI/Console.hpp>
#include <src/GUI/SceneEvents.hpp>

namespace brdfEditor {
namespace clRenderer {
class Scene;
}
namespace gui::editor {

// BRDF that is being edited.
struct BRDFEntry {
	// A Tab with this BRDF is open in the SourceEditor.
	bool opened;
	// Corresponds to the name in the materialLibrary
	std::string name;
	std::vector<char> source;
};

// Shared state of the Editor UI elements. Provides API to control edited BRDFs
// and materials.
class EditorParams {
public:
	// Saves a local copy of all BRDF sources, subsequent method calls act on
	// those copies.
	EditorParams(clRenderer::Scene &scene, SceneEvents &sceneEvents,
				 Console &console);
	// Returns: Vector of BRDFs that have been edited or atleast a tab with them
	// was opened.
	std::vector<BRDFEntry> &
	getEditedBRDFs();

	clRenderer::Scene &
	getScene() const;

	Console &
	getConsole() const;
	// Selects a BRDF for showing its parameters in BRDFMatList UI.
	// --name: Pass nullptr to deselect.
	void
	selectBRDF(const std::string *name);
	// See selectBRDF
	const std::optional<std::string> &
	getSelectedBRDF() const;
	// If called for the first time for this BRDF then it creates a BRDFEntry
	// entry in a vector returned by getEditedBRDFs. If called again then it
	// sets entry->opened=true.
	// - name: BRDF must be present in the scene.
	void
	editBRDF(const std::string &name);

	// Removes material from the scene. Must not be used by any objects in the
	// scene.Resets selectedMaterial if necessary.
	// --name: Must outlive this call, so it cannot be a reference to the key from material handle.
	// Returns: Whether the material was removed. False if the material with
	// such name did not exists or could not be removed. In the latter an error
	// message is printed on the console.
	bool
	removeMaterial(const std::string &name);
	// Removes BRDF and all associated materials from the scene. Those materials
	// must not be used by any objects in the scene. Resets
	// selected[BRDF/Material] if necessary.
	// --name: Must outlive this call, so it cannot be a reference to the key from BRDF handle.
	// Returns: Whether the BRDF was
	// removed. False if the BRDF with such name did not exists or could not be
	// removed. In the latter an error message is printed on the console.
	bool
	removeBRDF(const std::string &name);

	// Updates sources of all getEditedBRDFs() in the scene. Reparses them and
	// updates associated materials.
	// Returns: True if any of the sources were changed.
	// Throws: BRDF::FormatException if any source was incorrect so
	// BRDF::changeSource throwed.
	bool
	updateSources();
	// Selects a Material for showing its parameters in BRDFMatParams
	// --matHandle: Pass nullptr to deselect.
	void
	selectMaterial(const std::string *name);
	// See selectMaterial
	const std::optional<std::string> &
	getSelectedMaterial() const;

private:
	clRenderer::Scene *scene;
	SceneEvents *sceneEvents;
	Console *console;

	std::vector<BRDFEntry> editedBRDFs;
	std::optional<std::string> selectedBRDF;
	std::optional<std::string> selectedMaterial;
};
} // namespace gui::editor
} // namespace brdfEditor
#endif // !BRDFEDITOR_GUI_EDITING_EDITOR_PARAMS_HEADER
