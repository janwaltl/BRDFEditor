#ifndef BRDFEDITOR_OPENCL_RENDERER_RENDERER_EXEPTIONS_HEADER
#define BRDFEDITOR_OPENCL_RENDERER_RENDERER_EXEPTIONS_HEADER

#include <src/Core/Exception.hpp>

namespace brdfEditor::clRenderer {

class RendererException : public Exception {
	using Exception::Exception;
};

// Recompilation was requested so the kernel must be recompiled before calling
// the beginRender()
struct RecompilationNeeded : public Exception {
	using Exception::Exception;
};
struct InvalidRenderingDevice : public Exception {
	using Exception::Exception;
};
} // namespace brdfEditor::clRenderer
#endif // !BRDFEDITOR_OPENCL_RENDERER_RENDERER_PLATFORM_INFO_HEADER
