#include "MaterialsLibrary.hpp"

#include <algorithm>
#include <cassert>
#include <fstream>

#include <src/Core/Exception.hpp>
#include <src/OpenCLRenderer/Kernel/KernelSource.hpp>

namespace brdfEditor::clRenderer {
MaterialsLibrary::MaterialsLibrary() {}

bool
MaterialsLibrary::createMaterial(std::string name, BRDFsCHandle brdfHandle) {
	// Get non-const version
	// TODO ensure it is present here
	auto &&brdf = BRDFs.at(brdfHandle->first);
	bool inserted = materials.try_emplace(name, Material{brdfHandle}).second;
	if (!inserted)
		return false;

	brdf.materials.push_back(std::move(name));
	return true;
}
bool
MaterialsLibrary::createMaterial(std::string name, BRDFsHandle brdfHandle) {
	BRDF &brdf = brdfHandle->second;
	bool inserted = materials.try_emplace(name, Material{brdfHandle}).second;
	if (!inserted)
		return false;

	brdf.materials.push_back(std::move(name));
	return true;
}
bool
MaterialsLibrary::createMaterial(std::string name, BRDF brdf) {
	auto [IT, inserted] = BRDFs.try_emplace(name, std::move(brdf));
	if (!inserted)
		return false;
	inserted = createMaterial(std::move(name), IT);
	if (!inserted) { // Cleanup inserted BRDF
		BRDFs.erase(name);
		return false;
	}
	return true;
}
bool
MaterialsLibrary::createMaterial(std::string name, std::string kernelSource) {
	return createMaterial(std::move(name), BRDF{std::move(kernelSource), name});
}

bool
MaterialsLibrary::createBRDF(std::string name, std::string kernelSource) {
	auto [IT, inserted] = BRDFs.try_emplace(name, BRDF(std::move(kernelSource),
													   name));
	return inserted;
}

std::optional<BRDFsCHandle>
MaterialsLibrary::getBRDF(const std::string &name) const {
	auto IT = BRDFs.find(name);
	return IT != BRDFs.end() ? IT : std::optional<BRDFsCHandle>();
}

std::optional<BRDFsHandle>
MaterialsLibrary::getBRDF(const std::string &name) {
	auto IT = BRDFs.find(name);
	return IT != BRDFs.end() ? IT : std::optional<BRDFsHandle>();
}

std::optional<MaterialCHandle>
MaterialsLibrary::getMaterial(const std::string &name) const {
	auto IT = materials.find(name);
	return IT != materials.end() ? IT : std::optional<MaterialCHandle>{};
}
std::optional<MaterialHandle>
MaterialsLibrary::getMaterial(const std::string &name) {
	auto IT = materials.find(name);
	return IT != materials.end() ? IT : std::optional<MaterialHandle>{};
}
bool
MaterialsLibrary::removeMaterial(const std::string &name) {
	auto &&matIT = materials.find(name);
	if (matIT != materials.end()) {
		// Remove it from the associated BRDF.
		auto &&maybeBRDF = getBRDF(matIT->second.brdf->first);
		assert(maybeBRDF);
		auto &&brdfMaterials = (*maybeBRDF)->second.materials;
		brdfMaterials.erase(
			std::remove(brdfMaterials.begin(), brdfMaterials.end(), name));

		materials.erase(matIT);
		return true;
	}
	return false;
}
bool
MaterialsLibrary::removeBRDF(const std::string &name) {
	return BRDFs.erase(name) > 0;
}
materials_t &
MaterialsLibrary::getMaterials() {
	return materials;
}
const materials_t &
MaterialsLibrary::getMaterials() const {
	return materials;
}
BRDFS_t &
MaterialsLibrary::getBRDFs() {
	return BRDFs;
}
const BRDFS_t &
MaterialsLibrary::getBRDFs() const {
	return BRDFs;
}
} // namespace brdfEditor::clRenderer
