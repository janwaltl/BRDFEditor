#ifndef BRDFEDITOR_OPENCL_OBJECT_HEADER
#define BRDFEDITOR_OPENCL_OBJECT_HEADER

#include <string>

#include <src/OpenCLRenderer/Materials/MaterialsLibrary.hpp>

namespace brdfEditor::clRenderer {
// A renderable unit of a scene with name and material.
class Object {
public:
	Object(std::string name, MaterialCHandle mat);
	MaterialCHandle
	getMatHandle() const;
	void
	setMatHandle(MaterialCHandle newHandle);
	const std::string &
	getName() const;

protected: // RESOLVE protected or get/set?
	std::string name;
	MaterialCHandle mat;
};

} // namespace brdfEditor::clRenderer
#endif // !BRDFEDITOR_OPENCL_OBJECT_HEADER
