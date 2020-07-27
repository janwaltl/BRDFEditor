#ifndef BRDFEDITOR_WINDOW_MANAGER_HEADER
#define BRDFEDITOR_WINDOW_MANAGER_HEADER

#include <cstddef>
#include <string>
#include <optional>
#include <memory>

#include "Input.hpp"
#include "ImGuiManager.hpp"

struct GLFWwindow;
namespace brdfEditor {
class WindowContext;
// TODO Add Input capabilities - probably expose Input class
// Initializes OpenGL,GLAD and creates the main window.
class WindowManager {
public:
	WindowManager(const std::string &title, std::size_t width,
				  std::size_t height);
	~WindowManager();
	// Returns false if the window should be closed
	bool
	beginFrame();
	// Swaps the buffers
	void
	renderFrame();

	const WindowContext &
	getContext() const;
	std::pair<int, int>
	getWinSize() const;

private:
	void
	registerCallbacks();
	std::optional<std::string>
	getGLFWError();
	std::string
	getGLFWErrorOrUnknown();
	void static glfwErrorCallback(int error, const char *description);
	void static glfwKeyCallback(GLFWwindow *win, int key, int scancode,
								int action, int mods);
	void static glfwCharCallback(GLFWwindow *win, unsigned int unicodeKey);
	void static glfwMousePosCallback(GLFWwindow *win, double x, double y);
	void static glfwMouseButtonCallback(GLFWwindow *win, int button, int action,
										int mods);
	void static glfwMouseScrollCallback(GLFWwindow *win, double xScroll,
										double yScroll);
	// TODO drag&drop callback, clipboard
	void static glPostFncCallback(const char *fncName, void *fncPntr,
								  int numArgs, ...);
	void static glfwFrameBufferSizeCallback(GLFWwindow *win, int newW,
											int newH);
	GLFWwindow *win;
	Input input;
	std::unique_ptr<ImGuiManager> imGuiMan;
	std::unique_ptr<WindowContext> winContext;
};
/*THOUGHT Multi-window support
	In that case refactor this into Window and WindowBackend classes.*/
} // namespace brdfEditor
#endif // !BRDFEDITOR_WINDOW_MANAGER_HEADER
