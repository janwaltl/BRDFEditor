#include "Light.hpp"
namespace brdfEditor::clRenderer {
Light::Light(std::string name) : name(std::move(name)) {}

const std::string &
Light::getName() const {
	return name;
}

} // namespace brdfEditor::clRenderer
