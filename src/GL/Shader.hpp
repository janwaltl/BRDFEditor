#ifndef BRDFEDITOR_SHADER_HEADER
#define BRDFEDITOR_SHADER_HEADER

#include <string>

#include <glad/glad.h>

namespace brdfEditor {
// Represents one programmable stage of OpenGL pipeline.
class Shader {
public:
	enum class type {
		vertex = 1 << 0,
		fragment = 1 << 1,
		geometry = 1 << 2,
		compute = 1 << 3,
	};
	// Creates glShader object with passed source and compiles it.
	// Throws: ShaderException if the compilation fails.
	Shader(type t, const std::string &source);
	Shader(const Shader &) = delete;
	Shader(Shader &&) = default;
	Shader &
	operator=(const Shader &) = delete;
	Shader &
	operator=(Shader &&) = default;
	~Shader();

	void
	attachTo(GLint program) const;
	void
	detachFrom(GLint program) const;

private:
	GLuint ID = 0;
};
/*THOUGHT Make Shader class mutable.
	- Shader's source can be changed and the shader recompiled.
		- ShaderPrograms that have this shader still attached could then be
   relinked using the new version.
*/
} // namespace brdfEditor
#endif // !BRDFEDITOR_SHADER_HEADER
