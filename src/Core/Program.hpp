#ifndef BRDFEDITOR_PROGRAM_HEADER
#define BRDFEDITOR_PROGRAM_HEADER

#include <src/GUI/GUI.hpp>
#include <src/OpenCLRenderer/Renderer/Renderer.hpp>
#include <src/OpenCLRenderer/Scene.hpp>

#include "WindowManager.hpp"

namespace brdfEditor {
// The main class of this project
class Program {
public:
	Program();
	// Contains mains while(true) loop
	// Each frame GUI is drawn and, if not paused, renderer is called->one
	// sample is added to the texture.
	void
	Run();
	// Texture used by the renderer
	GLuint
	getTextureID() const {
		return texture;
	}
	glm::vec2
	getTextureDims() const;
	void
	setTextureDims(std::size_t width, std::size_t height);
	clRenderer::Scene &
	getScene() {
		return scene;
	}
	clRenderer::Renderer &
	getRenderer() {
		return renderer;
	}
	const clRenderer::Renderer &
	getRenderer() const {
		return renderer;
	}
	// Pauses rendering process, still draws GUI
	void
	pauseRendering();
	void
	resumeRendering();
	// Whether the rendering is paused or not.
	bool
	isRendering();
	// Get/Set maximum number of samples to render. After that the renderer will
	// pause.
	std::size_t &
	maxSamplesToRender();
	std::future<bool>
	saveImage(const std::string &filename, bool hdr);

private:
	struct SaveImgData {
		std::string filename;
		bool hdr;
		std::promise<bool> promise;
	};
	void
	processSaveImageRequests();
	WindowManager window;
	clRenderer::Scene scene; // Possible multiple scenes
	GLuint texture;
	clRenderer::Renderer renderer;
	gui::GUI gui;
	// Whether the rendering is paused or running.
	bool rendering;
	std::vector<SaveImgData> saveImageRequests;
	std::size_t maxNumSamplesToRender;
};
} // namespace brdfEditor

#endif