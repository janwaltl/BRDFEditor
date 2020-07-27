#include "Material.hpp"

#include <cassert>
#include <fstream>

#include <src/OpenCLRenderer/Kernel/KernelSource.hpp>

namespace brdfEditor::clRenderer {

Material::Material(BRDFsCHandle brdf) : brdf(brdf) {
	// Creates material from their BRDF description.
	this->params = brdf->second.parameters;
}
} // namespace brdfEditor::clRenderer
