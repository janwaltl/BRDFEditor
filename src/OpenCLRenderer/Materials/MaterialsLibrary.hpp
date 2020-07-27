#ifndef BRDFEDITOR_OPENCL_MATERIALS_LIBRARY_HEADER
#define BRDFEDITOR_OPENCL_MATERIALS_LIBRARY_HEADER

#include <map>
#include <optional>
#include <string>

#include <src/OpenCLRenderer/Kernel/BRDF.hpp>

#include "Material.hpp"

namespace brdfEditor::clRenderer {

// Collection of materials and their sources.
class MaterialsLibrary {
public:
	// Fills the library with basic materials.
	MaterialsLibrary();

	// Creates a material from the already present source.
	// --brdfHandle: Must be obtained and present in this instance's materials.
	// returns: Whether the material was created. Returns false if a material
	// with given name already exists.
	bool
	createMaterial(std::string name, BRDFsCHandle brdfHandle);
	// See BRDFsCHandle overload.
	bool
	createMaterial(std::string name, BRDFsHandle brdfHandle);
	// Adds a new material and its source to the library.
	// returns: Whether the BRDF and material were created & inserted. Returns
	// false if either a BRDF or material with the same name exists. In that
	// case neither is inserted.
	bool
	createMaterial(std::string name, BRDF brdf);
	// Adds a new material and its source to the library.
	// Material source is also created with the same name.
	// returns: Whether the BRDF and material were created & inserted. Returns
	// false if either a BRDF or material with the same name exists. In that
	// case neither is inserted.
	bool
	createMaterial(std::string name, std::string kernelSource);
	// Adds a new BRDF and its source tot the library.
	// Returns: Whether the BRDF was created and inserted. Retuns false if a
	// BRDF with the same name exists.
	bool
	createBRDF(std::string name, std::string kernelSource);
	// Returns handle to the BRDF with the given name if it exists. The handle
	// is valid until this library exists and the material has not been removed.
	std::optional<BRDFsCHandle>
	getBRDF(const std::string &name) const;
	// See const version.
	std::optional<BRDFsHandle>
	getBRDF(const std::string &name);
	// Returns handle to the BRDF with the given name. The handle is valid
	// until this library exists and the material has not been removed.
	std::optional<MaterialCHandle>
	getMaterial(const std::string &name) const;
	// See const overload.
	std::optional<MaterialHandle>
	getMaterial(const std::string &name);
	// Returns: Whether the material was removed. False if a material with such
	// name did not exist.
	bool
	removeMaterial(const std::string &name);
	// Returns: Whether the material was removed. False if a BRDF with such name
	// did not exist.
	bool
	removeBRDF(const std::string &name);
	materials_t &
	getMaterials();

	const materials_t &
	getMaterials() const;

	BRDFS_t &
	getBRDFs();

	const BRDFS_t &
	getBRDFs() const;

private:
	materials_t materials;
	BRDFS_t BRDFs;
};
} // namespace brdfEditor::clRenderer
#endif // !BRDFEDITOR_OPENCL_MATERIALS_MANAGER