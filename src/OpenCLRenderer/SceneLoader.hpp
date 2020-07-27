#ifndef BRDFEDITOR_RENDERER_SCENE_LOADER_HEADER
#define BRDFEDITOR_RENDERER_SCENE_LOADER_HEADER

#include <string>

#include <src/Core/Exception.hpp>
#include <src/OpenCLRenderer/Scene.hpp>

namespace brdfEditor::clRenderer {

class SceneFormatException : public Exception {
public:
	using Exception::Exception;
};

// Loads and returns a scene loaded from passed json file.
// --filename: Full path including extension
Scene
loadSceneFromFile(const std::string &filename);
} // namespace brdfEditor::clRenderer
#endif // !BRDFEDITOR_RENDERER_SCENE_LOADER_HEADER
