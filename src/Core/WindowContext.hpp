#ifndef BRDFEDITOR_CORE_WINDOW_CONTEXT_HEADER
#define BRDFEDITOR_CORE_WINDOW_CONTEXT_HEADER

// TODO other platforms
#ifdef WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#endif
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

struct GLFWwindow;

namespace brdfEditor {
// Holds handles to the OpenGL and window contexts.
class WindowContext {
public:
	enum DisplayType {
		WGL,
		// TODO otherPlatforms
	};
#ifdef WIN32
	WindowContext(GLFWwindow *win) : win(win) {}
	auto
	getGLContext() const {
		return glfwGetWGLContext(win);
	}
	auto
	getDisplayContext() const {
		return GetDC(glfwGetWin32Window(win));
	}
	static DisplayType
	getDisplayType() {
		return WGL;
	}
#endif
private:
	GLFWwindow *win;
};
} // namespace brdfEditor
#endif // !BRDFEDITOR_CORE_WINDOW_CONTEXT_HEADER
