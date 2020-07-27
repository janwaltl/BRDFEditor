#include "Object.hpp"

namespace brdfEditor::clRenderer {
Object::Object(std::string name, MaterialCHandle mat) :
	name(std::move(name)),
	mat(mat) {}

MaterialCHandle
Object::getMatHandle() const {
	return mat;
}
void
Object::setMatHandle(MaterialCHandle newHandle) {
	mat = newHandle;
}
const std::string &
Object::getName() const {
	return name;
}
} // namespace brdfEditor::clRenderer
