#ifndef BRDFEDITOR_SHADER_EXCEPTION_HEADER
#define BRDFEDITOR_SHADER_EXCEPTION_HEADER

#include <src/Core/Exception.hpp>

namespace brdfEditor {
class ShaderException : public Exception {
public:
	using Exception::Exception;
};
} // namespace brdfEditor
#endif // !BRDFEDITOR_SHADER_EXCEPTION_HEADER
