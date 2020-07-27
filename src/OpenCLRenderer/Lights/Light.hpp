#ifndef BRDFEDITOR_OPENCL_LIGHT_HEADER
#define BRDFEDITOR_OPENCL_LIGHT_HEADER

#include <string>

namespace brdfEditor::clRenderer {
	class Light {
	public:
		Light(std::string name);
		
		const std::string& getName()const;
	private:
		std::string name;
	};
} // namespace brdfEditor::clRenderer
#endif // !BRDFEDITOR_OPENCL_LIGHT_HEADER