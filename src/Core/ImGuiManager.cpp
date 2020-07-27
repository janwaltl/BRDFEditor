#include "ImGuiManager.hpp"

#include <string>

#include <external/ImGui/imgui.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace brdfEditor {
namespace {
void
setImGuiKeyMap() {
	ImGuiIO &io = ImGui::GetIO();
	io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
	io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
	io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
	io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
	io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
	io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
	io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
	io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
	io.KeyMap[ImGuiKey_Insert] = GLFW_KEY_INSERT;
	io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
	io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
	io.KeyMap[ImGuiKey_Space] = GLFW_KEY_SPACE;
	io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
	io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
	io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
	io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
	io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
	io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
	io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
	io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;
}
GLuint
loadFontTexture() {
	ImGuiIO &io = ImGui::GetIO();
	unsigned char *pixels;
	int w, h;
	io.Fonts->AddFontFromFileTTF("Font/Tuffy_Bold.ttf", 16.0f);

	// io.FontGlobalScale *= 2.0f;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &w, &h);

	GLuint texID;
	glCreateTextures(GL_TEXTURE_2D, 1, &texID);
	// No mipmaps, because its an atlas of glyphs
	glTextureStorage2D(texID, 1, GL_RGBA8, w, h);
	glTextureParameteri(texID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(texID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureSubImage2D(texID, 0, 0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE,
						pixels);
	glGenerateTextureMipmap(texID);
	io.Fonts->TexID = reinterpret_cast<void *>(texID);
	return texID;
}
void
setOpenGLSettings() {
	// RESOLVE These settings could potentially clash with other rendering.
	//	- If the scene would be rendered with OpenGL then depth test should be
	// enabled.
	//		- So call this in renderData or add some kind of OpenGLStateManager
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_CULL_FACE);
}
constexpr const char *vertSource = R"(
			#version 450
			layout(location=0) in vec2 pos;
			layout(location=1) in vec2 UV;
			layout(location=2) in vec4 col;
			
			uniform mat4 ortoMat;
			
			out vec2 fUV;
			out vec4 fCol;
			void main()
			{
				fUV=UV;
				fCol=col;
				gl_Position=ortoMat*vec4(pos,0.0f,1.0f);
			})";
constexpr const char *fragSource = R"(
			#version 450
			
			in vec2 fUV;
			in vec4 fCol;
			
			layout(binding = 0) uniform sampler2D tex;
			
			out vec4 color;
			void main()
			{
				color = fCol * texture(tex,fUV);
				float gamma = 2.2;
				color.xyz = pow(color.xyz,vec3(1.0/gamma));
			})";
void
initGLBuffers(GLuint &VAO, GLuint &VBO, GLuint &IBO) {
	glCreateVertexArrays(1, &VAO);
	glCreateBuffers(1, &VBO);
	glCreateBuffers(1, &IBO);
	// pos
	glEnableVertexArrayAttrib(VAO, 0);
	glVertexArrayAttribBinding(VAO, 0, 0);
	glVertexArrayAttribFormat(VAO, 0, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayVertexBuffer(VAO, 0, VBO, offsetof(ImDrawVert, pos),
							  sizeof(ImDrawVert));
	// UV
	glEnableVertexArrayAttrib(VAO, 1);
	glVertexArrayAttribBinding(VAO, 1, 1);
	glVertexArrayAttribFormat(VAO, 1, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayVertexBuffer(VAO, 1, VBO, offsetof(ImDrawVert, uv),
							  sizeof(ImDrawVert));
	// col
	glEnableVertexArrayAttrib(VAO, 2);
	glVertexArrayAttribBinding(VAO, 2, 2);
	glVertexArrayAttribFormat(VAO, 2, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0);
	glVertexArrayVertexBuffer(VAO, 2, VBO, offsetof(ImDrawVert, col),
							  sizeof(ImDrawVert));

	glVertexArrayElementBuffer(VAO, IBO);
}

static const char *
getClipBoardStrCallback(void *glfwWin) {
	return glfwGetClipboardString((GLFWwindow *)glfwWin);
}

static void
setClipBoardStrCallback(void *glfwWin, const char *str) {
	glfwSetClipboardString((GLFWwindow *)glfwWin, str);
}

static void
setGUIStyle() {


	ImGui::StyleColorsDark ();
	ImVec4 *colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_TitleBg] = colors[ImGuiCol_TitleBgActive];
#if true
	colors[ImGuiCol_Text] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.12f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
	colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.30f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 1.00f, 1.00f, 0.51f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.78f, 0.78f, 0.78f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.98f, 0.98f, 0.98f, 0.53f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.69f, 0.69f, 0.69f, 0.80f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.49f, 0.49f, 0.49f, 0.80f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.46f, 0.54f, 0.80f, 0.60f);
	colors[ImGuiCol_Button] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_Separator] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.14f, 0.44f, 0.80f, 0.78f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.14f, 0.44f, 0.80f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.80f, 0.80f, 0.80f, 0.56f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.45f, 0.00f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.70f, 0.70f, 0.70f, 0.70f);
#endif //  false
}
} // namespace
ImGuiManager::ImGuiManager(GLFWwindow *win) :
	shader(Shader(Shader::type::vertex, vertSource),
		   Shader(Shader::type::fragment, fragSource), true) {
	ImGui::CreateContext();
	// Texture for the drawing render function
	shader.setUniform("tex", 0);
	setImGuiKeyMap();

	texID = loadFontTexture();
	setOpenGLSettings();
	setGUIStyle();
	auto &&io = ImGui::GetIO();
	io.SetClipboardTextFn = setClipBoardStrCallback;
	io.GetClipboardTextFn = getClipBoardStrCallback;
	io.ClipboardUserData = win;

	// For 4K
	// ImGui::GetStyle().ScaleAllSizes(2.0f);

	initGLBuffers(VAO, VBO, IBO);
}
ImGuiManager::~ImGuiManager() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &IBO);
	glDeleteTextures(1, &texID);
	ImGui::DestroyContext();
}
ImGuiManager::captures
ImGuiManager::newFrame(float deltaT, float mouseX, float mouseY) {
	ImGuiIO &io = ImGui::GetIO();
	io.MousePos = {mouseX, mouseY};
	io.DeltaTime = deltaT;
	io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] ||
				 io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
	io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] ||
				  io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
	io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] ||
				io.KeysDown[GLFW_KEY_RIGHT_ALT];
	io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] ||
				  io.KeysDown[GLFW_KEY_RIGHT_SUPER];
	// All input should have been captured by now by GLFW callbacks and passed
	// directly to ImGui.
	ImGui::NewFrame();
	return {io.WantCaptureMouse, io.WantCaptureKeyboard};
}
void
ImGuiManager::render() {
	ImGui::Render();
	renderData(ImGui::GetDrawData());
}
void
ImGuiManager::renderData(ImDrawData *draw_data) {
	glEnable(GL_SCISSOR_TEST);

	// Written according to documentation in imgui.h/imgui.cpp and the example
	// at: https://github.com/ocornut/imgui/tree/master/examples/opengl3_example
	ImGuiIO &io = ImGui::GetIO();
	draw_data->ScaleClipRects(io.DisplayFramebufferScale);
	const GLfloat ortoMat[4][4] = {
		{2.0f / io.DisplaySize.x, 0.0f, 0.0f, 0.0f},
		{0.0f, 2.0f / -io.DisplaySize.y, 0.0f, 0.0f},
		{0.0f, 0.0f, -1.0f, 0.0f},
		{-1.0f, 1.0f, 0.0f, 1.0f},
	};
	shader.setUniform("ortoMat", ortoMat);
	shader.bind();
	for (int i = 0; i < draw_data->CmdListsCount; i++) {
		ImDrawList *cmdList = draw_data->CmdLists[i];
		const ImDrawIdx *idx_buffer_offset = 0;

		glNamedBufferData(VBO, cmdList->VtxBuffer.Size * sizeof(ImDrawVert),
						  cmdList->VtxBuffer.Data, GL_STREAM_DRAW);
		glNamedBufferData(IBO, cmdList->IdxBuffer.Size * sizeof(ImDrawIdx),
						  cmdList->IdxBuffer.Data, GL_STREAM_DRAW);
		// Apparently on Intel GPUs Element buffer is not part of VAO?!
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
		glBindVertexArray(VAO);
		for (int cmd_i = 0; cmd_i < cmdList->CmdBuffer.Size; cmd_i++) {
			ImDrawCmd *pcmd = &cmdList->CmdBuffer[cmd_i];
			glBindTextureUnit(0, reinterpret_cast<GLuint>(pcmd->TextureId));
			float height = io.DisplaySize.y * io.DisplayFramebufferScale.y;
			// RESOLVE flickers when enabled, wrong clipping when disabled.
			glScissor((int)pcmd->ClipRect.x, (int)(height - pcmd->ClipRect.w),
					  (int)(pcmd->ClipRect.z - pcmd->ClipRect.x),
					  (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));
			glDrawElements(GL_TRIANGLES, pcmd->ElemCount, GL_UNSIGNED_SHORT,
						   idx_buffer_offset);
			idx_buffer_offset += pcmd->ElemCount;
		}
		glBindVertexArray(0);
	}
	glDisable(GL_SCISSOR_TEST);
}

} // namespace brdfEditor