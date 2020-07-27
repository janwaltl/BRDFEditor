#ifndef BRDFEDITOR_OPENCL_MATERIAL_HEADER
#define BRDFEDITOR_OPENCL_MATERIAL_HEADER

#include <string>
#include <map>
#include <memory>

#include <src/OpenCLRenderer/CLHeaders.hpp>
#include <src/OpenCLRenderer/Kernel/BRDF.hpp>
#include <src/OpenCLRenderer/Kernel/BRDFParams.h>

namespace brdfEditor::clRenderer {

// Material that can be attached to a scene object.
class Material {
public:
	// Creates named material with given source.
	Material(BRDFsCHandle brdf);
	BRDFParams_t params;
	BRDFsCHandle brdf;
};

using materials_t = std::map<std::string, Material>;
// Small value type, cheap copy, move. random-access iterator.
// Valid until the material(or the library) exists.
using MaterialHandle = materials_t::iterator;
// See MaterialHandle
using MaterialCHandle = materials_t::const_iterator;
} // namespace brdfEditor::clRenderer
#endif // !BRDFEDITOR_OPENCL_MATERIAL_HEADER
