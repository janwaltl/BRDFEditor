#include "Program.hpp"

#include <stb/stb_image_write.h>
#include <glad/glad.h>
#include <ImGui/imgui.h>

#include <src/OpenCLRenderer/Scene.hpp>

#include "WindowContext.hpp"
#include <src/OpenCLRenderer/SceneLoader.hpp>

namespace brdfEditor {
namespace {
constexpr const std::size_t w = 1280;
constexpr const std::size_t h = 768;

constexpr const std::size_t imageSizeX = 1280, imageSizeY = 1280;
GLuint
initTexture(std::size_t w, std::size_t h) {
	GLuint texture;
	glCreateTextures(GL_TEXTURE_2D, 1, &texture);
	glTextureStorage2D(texture, 1, GL_RGBA32F, w, h);
	glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// RESOLVE Is this necessary?
	std::vector<float> pixels(w * h * 4, 1.0f);
	glTextureSubImage2D(texture, 0, 0, 0, w, h, GL_RGBA, GL_FLOAT,
						pixels.data());
	return texture;
}

clRenderer::Scene
buildTestScene() {
	using namespace clRenderer;
	Scene scene = loadSceneFromFile("Scenes/defaultScene.json");

	return scene;
}
} // namespace
Program::Program() :
	window("BRDF Editor - Bachelor Thesis, Jan Waltl", w, h),
	scene(buildTestScene()),
	texture(initTexture(imageSizeX, imageSizeY)),
	renderer(&window.getContext(), scene, texture, imageSizeX, imageSizeY),
	gui(w, h, *this),
	rendering(false), // Start rendering immidietely after a device is set
	maxNumSamplesToRender(1000) {}

void
Program::Run() {
	// TEMP Defaults to GPU on my computer
	//renderer.setDevice(&renderer.getAvailablePlatforms()[0].devices[0]);
	while (!window.beginFrame()) {
		bool began = false;

		bool finished = maxNumSamplesToRender <= renderer.getNumSamples();

		// If the rendering is finished, pause.
		// If the user then changes anything, resume.
		if (finished && !renderer.isRecompilationRequested()) {
			this->pauseRendering();
		} else if (finished) {
			this->resumeRendering();
		}
		try {
			if (rendering && renderer.getCurrentDevice()) // A device is set
			{
				if (renderer.isRecompilationRequested())
					renderer.recompile( nullptr,nullptr, nullptr, &scene);
				renderer.beginRender();
				began = true;
			}
		} catch (const Exception &e) {
			this->pauseRendering();
			gui.getConsole().printLine(
				std::string("Renderer error: Recompilation failed, reason: ") +
				e.what());
		}

		auto [x, y] = window.getWinSize();
		gui.draw(float(x), float(y));

		try
		{
		if (began)
			renderer.finishRender();
		}
		catch (const std::exception& e) {
			this->pauseRendering();
			gui.getConsole().printLine(
				std::string("Renderer error: ") +
				e.what());
		}

		processSaveImageRequests();

		window.renderFrame();
	}
}
glm::vec2
Program::getTextureDims() const {
	return {(float)renderer.getTextureWidth(),
			(float)renderer.getTextureHeight()};
}
void
Program::setTextureDims(std::size_t width, std::size_t height) {
	glDeleteTextures(1, &texture);
	texture = initTexture(width, height);

	renderer.recompile(&width, &height, &texture, nullptr);
}
void
Program::pauseRendering() {
	rendering = false;
}
void
Program::resumeRendering() {
	rendering = true;
}
bool
Program::isRendering() {
	return rendering;
}

std::size_t &
Program::maxSamplesToRender() {
	return maxNumSamplesToRender;
}

std::future<bool>
Program::saveImage(const std::string &filename, bool hdr) {
	auto &&imgData = saveImageRequests.emplace_back();
	imgData.filename = filename + ".bmp";
	imgData.hdr = hdr;

	return imgData.promise.get_future();
}

void
Program::processSaveImageRequests() {
	if (saveImageRequests.empty())
		return;
	GLint boundTexture;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &boundTexture);
	glBindTexture(GL_TEXTURE_2D, texture);

	auto pixels = std::make_unique<uint8_t[]>(renderer.getTextureWidth() *
											  renderer.getTextureHeight() * 3);

	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels.get());
	glBindTexture(GL_TEXTURE_2D, boundTexture);
	//Gamma correction
	for (std::size_t i = 0; i < renderer.getTextureWidth() *renderer.getTextureHeight() * 3; ++i) {
		double p = pixels[i] / 255.0;
		p = std::clamp(std::pow(p, 1 / 2.2),0.0,1.0);
		pixels[i] = uint8_t(p * 255.0);
	} 
	for (auto &&[filename, isHDR, promise] : saveImageRequests) {
		int res = stbi_write_bmp(filename.c_str(), renderer.getTextureWidth(),
								 renderer.getTextureHeight(), 3, pixels.get());
		promise.set_value(res != 0);
	}
	saveImageRequests.clear();
}
} // namespace brdfEditor
