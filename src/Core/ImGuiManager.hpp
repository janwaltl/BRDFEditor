#ifndef BRDFEDITOR_IMGUI_MANAGER_HEADER
#define BRDFEDITOR_IMGUI_MANAGER_HEADER

#include <src/GL/ShaderProgram.hpp>

struct ImDrawData;
struct GLFWwindow;

namespace brdfEditor {
// Initializes ImGui library.
// Only 1 is allowed currently.(Because ImGui context is global)
// Requires valid OpenGL context at construction and destruction.
class ImGuiManager {
public:
	struct captures {
		bool mouse, keyboard;
	};
	ImGuiManager(GLFWwindow *win);
	~ImGuiManager();
	ImGuiManager(const ImGuiManager &) = delete;
	ImGuiManager(ImGuiManager &&) = default;
	ImGuiManager &
	operator=(const ImGuiManager &) = delete;
	ImGuiManager &
	operator=(ImGuiManager &&) = default;
	// Prepares ImGui for drawing.
	// --mouseX,mouseY: Current position of the mouse.
	// Returns whether the ImGui wants to capture mouse or keyboard.
	captures
	newFrame(float deltaT, float mouseX, float mouseY);
	void
	render();

private:
	void
	renderData(ImDrawData *draw_data);
	ShaderProgram shader;
	GLuint texID;
	GLuint VAO;
	GLuint VBO;
	GLuint IBO;
};
} // namespace brdfEditor

#endif