#ifndef BRDFEDITOR_SHADER_PROGRAM_HEADER
#define BRDFEDITOR_SHADER_PROGRAM_HEADER

#include <map>
#include <string>
#include <vector>

#include <glad/glad.h>

#include "Shader.hpp"

namespace brdfEditor {
// Represents glProgram built and linked from Shaders.
class ShaderProgram {
public:
	// Creates glProgram, attaches passed shaders to it and links them together.
	// It will optionally also detach passed shaders from the linked program.
	// Throws: ShaderException if shaders could not be linked together fails.
	ShaderProgram(const std::vector<const Shader *> &shaders,
				  bool detachShaders = true);
	ShaderProgram(const std::vector<Shader> &shaders);
	ShaderProgram(const Shader &shader, bool detachShader = true);
	ShaderProgram(const Shader &shader1, const Shader &shader2,
				  bool detachShaders = true);

	ShaderProgram(const ShaderProgram &) = delete;
	ShaderProgram(ShaderProgram &&) = default;
	ShaderProgram &
	operator=(const ShaderProgram &) = delete;
	ShaderProgram &
	operator=(ShaderProgram &&) = default;
	// RESOLVE Keep these functions const and reserve non-const for modifying
	// the source?

	void
	bind() const;
	void
	unBind() const;
	void
	setUniform(const std::string &name, GLint x) const;
	void
	setUniform(const std::string &name, GLint x, GLint y) const;
	void
	setUniform(const std::string &name, GLint x, GLint y, GLint z) const;
	void
	setUniform(const std::string &name, GLint x, GLint y, GLint z,
			   GLint w) const;
	void
	setUniform(const std::string &name, GLfloat x) const;
	void
	setUniform(const std::string &name, GLfloat x, GLfloat y) const;
	void
	setUniform(const std::string &name, GLfloat x, GLfloat y, GLfloat z) const;
	void
	setUniform(const std::string &name, GLfloat x, GLfloat y, GLfloat z,
			   GLfloat w) const;
	void
	setUniform(const std::string &name, const GLfloat matrix[4][4]) const;
	void
	setUniformSampler(const std::string &name, GLuint texID) const;

	// IMPROVE Subrutines, explicit textures... if needed.
private:
	void
	saveUniformLocations();
	GLint
	getUniformLocation(const std::string &name) const;
	GLint
	getUniformLocOrThrow(const std::string &name) const;
	GLuint ID = 0;
	// Holds cached locations of all the active uniforms in the shader program.
	// OpenGL guarantees they do not change throughout the lifetime of the
	// program.
	std::map<std::string, GLint> uniformLocations;
};

// THOUGHT Allow relinking of ShaderProgram with new version of attached shaders
// or attach new ones.
} // namespace brdfEditor

#endif