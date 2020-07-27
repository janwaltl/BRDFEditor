#include "WindowManager.hpp"

#include <external/ImGui/imgui.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <src/GL/GLException.hpp>

#include "Exception.hpp"
#include "WindowContext.hpp"

namespace brdfEditor {
namespace {
// GLFW'state is global anyway, so no harm in global variables.
std::string errMsg;
int errCode;
bool hasError = false;
} // namespace
WindowManager::WindowManager(const std::string &title, std::size_t width,
							 std::size_t height) {
	glfwSetErrorCallback(&WindowManager::glfwErrorCallback);
	if (!glfwInit())
		throw Exception("GLFW Initialization failed, reason: " +
						getGLFWErrorOrUnknown());

	// RESOLVE Which version to target?
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	win = glfwCreateWindow(static_cast<int>(width), static_cast<int>(height),
						   title.c_str(), NULL, NULL);
	if (!win) {
		throw Exception("Failed to create the window, reason: " +
						getGLFWErrorOrUnknown());
		glfwTerminate();
	}
	glfwMakeContextCurrent(win);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		throw Exception(
			"GLAD initialization failed."); // RESOLVE Probably older version
	registerCallbacks();

	// TEMP Turn on VSync
	//glfwSwapInterval(1);

	
	int w, h;
	glfwGetFramebufferSize(win, &w, &h);
	glViewport(0, 0, w, h);

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

	imGuiMan = std::make_unique<ImGuiManager>(win);
	ImGui::GetIO().DisplaySize = ImVec2(static_cast<float>(w),
										static_cast<float>(h));
	winContext = std::make_unique<WindowContext>(win);
}
WindowManager::~WindowManager() {
	// Delete resource before deleting the OpenGL context
	imGuiMan.reset();
	glfwTerminate();
}
std::optional<std::string>
WindowManager::getGLFWError() {
	if (hasError) {
		hasError = false;
		return {std::to_string(errCode) + ':' + std::move(errMsg)};
	} else
		return {};
}
std::string
WindowManager::getGLFWErrorOrUnknown() {
	auto err = getGLFWError();
	if (err)
		return *err;
	else
		return "UNKNOWN";
}

bool
WindowManager::beginFrame() {
	bool shouldClose = glfwWindowShouldClose(win);
	glfwPollEvents();
	auto [x, y] = input.getMousePos();
	// TODO pause Input when captures are true, correct deltaTime
	imGuiMan->newFrame(1 / 60.0f, static_cast<float>(x), static_cast<float>(y));
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	return shouldClose;
}
void
WindowManager::renderFrame() {
	imGuiMan->render();
	glfwSwapBuffers(win);
}
const WindowContext &
WindowManager::getContext() const {
	return *winContext.get();
}
std::pair<int, int>
WindowManager::getWinSize() const {
	int w, h;
	
	glfwGetWindowSize(win, &w, &h);
	return {w, h};
}
void
WindowManager::registerCallbacks() {
	glfwSetWindowUserPointer(win, this);
	glad_set_post_callback(&WindowManager::glPostFncCallback);
	glfwSetFramebufferSizeCallback(win,
								   &WindowManager::glfwFrameBufferSizeCallback);
	glfwSetKeyCallback(win, &WindowManager::glfwKeyCallback);
	glfwSetCharCallback(win, &WindowManager::glfwCharCallback);
	glfwSetCursorPosCallback(win, &WindowManager::glfwMousePosCallback);
	glfwSetMouseButtonCallback(win, &WindowManager::glfwMouseButtonCallback);
	glfwSetScrollCallback(win, &WindowManager::glfwMouseScrollCallback);
}

void
WindowManager::glfwErrorCallback(int error, const char *description) {
	hasError = true;
	errMsg = description;
	errCode = error;
}

// RESOLVE Cleaner ImGui input catching, preferably in ImGuiManager, not here.
//	- Just create equivalents of callbacks in ImGuiMan and call them from here.

void
WindowManager::glfwKeyCallback(GLFWwindow *win, int key, int scancode,
							   int action, int mods) {
	WindowManager *man = reinterpret_cast<WindowManager *>(
		glfwGetWindowUserPointer(win));

	Input::state newState = Input::state::release;
	if (action == GLFW_PRESS)
		newState = Input::state::press;
	else if (action == GLFW_REPEAT)
		newState = Input::state::repeat;
	man->input.setKey(key, newState, PassKey<WindowManager>());

	if (action == GLFW_RELEASE)
		ImGui::GetIO().KeysDown[key] = false;
	else
		ImGui::GetIO().KeysDown[key] = true;
}
void
WindowManager::glfwCharCallback(GLFWwindow *win, unsigned int unicodeKey) {
	if (unicodeKey < (unsigned int)std::numeric_limits<ImWchar>::max())
		ImGui::GetIO().AddInputCharacter(static_cast<ImWchar>(unicodeKey));
}
void
WindowManager::glfwMousePosCallback(GLFWwindow *win, double x, double y) {
	WindowManager *man = reinterpret_cast<WindowManager *>(
		glfwGetWindowUserPointer(win));
	man->input.setMousePos({x, y}, PassKey<WindowManager>());
}
void
WindowManager::glfwMouseButtonCallback(GLFWwindow *win, int button, int action,
									   int mods) {
	WindowManager *man = reinterpret_cast<WindowManager *>(
		glfwGetWindowUserPointer(win));
	Input::state newState = Input::state::release;
	if (action == GLFW_PRESS)
		newState = Input::state::press;
	else if (action == GLFW_REPEAT)
		newState = Input::state::repeat;
	Input::mouseB mButton;
	if (button == GLFW_MOUSE_BUTTON_LEFT)
		mButton = Input::mouseB::left;
	else if (button == GLFW_MOUSE_BUTTON_MIDDLE)
		mButton = Input::mouseB::middle;
	else if (button == GLFW_MOUSE_BUTTON_RIGHT)
		mButton = Input::mouseB::right;
	else
		return; // Ignore other mouse buttons
	man->input.setMouseButton(mButton, newState, PassKey<WindowManager>());

	if (button >= 0 && button < 3) {
		if (action == GLFW_RELEASE)
			ImGui::GetIO().MouseDown[button] = false;
		else
			ImGui::GetIO().MouseDown[button] = true;
	}
}
void
WindowManager::glfwMouseScrollCallback(GLFWwindow *win, double xScroll,
									   double yScroll) {
	WindowManager *man = reinterpret_cast<WindowManager *>(
		glfwGetWindowUserPointer(win));
	man->input.setMouseScroll({xScroll, yScroll}, PassKey<WindowManager>());
	ImGuiIO &io = ImGui::GetIO();
	io.MouseWheelH += static_cast<float>(xScroll);
	io.MouseWheel += static_cast<float>(yScroll);
}

void
WindowManager::glPostFncCallback(const char *fncName, void *fncPntr,
								 int numArgs, ...) {
	GLenum error_code = glad_glGetError();
	if (error_code != GL_NO_ERROR)
		throw GLError(GLError::glEnumToError(error_code), fncName);
}
void
WindowManager::glfwFrameBufferSizeCallback(GLFWwindow *win, int newW,
										   int newH) {
	glViewport(0, 0, newW, newH);
	ImGui::GetIO().DisplaySize = ImVec2(static_cast<float>(newW),
		static_cast<float>(newH));
}

} // namespace brdfEditor