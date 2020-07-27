#include "ShaderProgram.hpp"

#include <memory>

#include "ShaderException.hpp"

namespace brdfEditor {
namespace {
constexpr const GLint invalidUniformLoc = -1;

std::vector<const Shader *>
GetShaderPtrs(const std::vector<Shader> &shaders) {
	std::vector<const Shader *> shaderPtrs;
	for (const Shader &shader : shaders) {
		shaderPtrs.push_back(&shader);
	}
	return shaderPtrs;
}
} // namespace
ShaderProgram::ShaderProgram(const Shader &shader, bool detachShader) :
	ShaderProgram({&shader}, detachShader) {}

ShaderProgram::ShaderProgram(const Shader &shader1, const Shader &shader2,
							 bool detachShaders) :
	ShaderProgram({&shader1, &shader2}, detachShaders) {}

ShaderProgram::ShaderProgram(const std::vector<Shader> &shaders) :
	ShaderProgram(GetShaderPtrs(shaders), true) {}

ShaderProgram::ShaderProgram(const std::vector<const Shader *> &shaders,
							 bool detachShaders) {
	ID = glCreateProgram();
	if (ID == 0)
		throw ShaderException("Could not create glProgram object.");
	for (const Shader *s : shaders)
		s->attachTo(ID);
	glLinkProgram(ID);

	GLint status;
	glGetProgramiv(ID, GL_LINK_STATUS, &status);
	if (status != GL_TRUE) {
		GLint length;
		glGetProgramiv(ID, GL_INFO_LOG_LENGTH, &length);
		auto log = std::make_unique<GLchar[]>(length);
		glGetProgramInfoLog(ID, length, nullptr, log.get());
		throw ShaderException(
			std::string("Program linking has failed, log:\n") + log.get());
	}

	if (detachShaders)
		for (const Shader *s : shaders)
			s->detachFrom(ID);

	saveUniformLocations();
}

void
ShaderProgram::bind() const {
	glUseProgram(ID);
}
void
ShaderProgram::unBind() const {
	glUseProgram(0);
}

// IMPROVE Also cache uniform's type and throw if the user uses incorrect
// overload?
//	- Unchecked way just generates GL_INVALID_OPERATION and changes nothing.

void
ShaderProgram::setUniform(const std::string &name, GLint x) const {
	glProgramUniform1i(ID, getUniformLocOrThrow(name), x);
}
void
ShaderProgram::setUniform(const std::string &name, GLint x, GLint y) const {
	glProgramUniform2i(ID, getUniformLocOrThrow(name), x, y);
}
void
ShaderProgram::setUniform(const std::string &name, GLint x, GLint y,
						  GLint z) const {
	glProgramUniform3i(ID, getUniformLocOrThrow(name), x, y, z);
}
void
ShaderProgram::setUniform(const std::string &name, GLint x, GLint y, GLint z,
						  GLint w) const {
	glProgramUniform4i(ID, getUniformLocOrThrow(name), x, y, z, w);
}
void
ShaderProgram::setUniform(const std::string &name, GLfloat x) const {
	glProgramUniform1f(ID, getUniformLocOrThrow(name), x);
}
void
ShaderProgram::setUniform(const std::string &name, GLfloat x, GLfloat y) const {
	glProgramUniform2f(ID, getUniformLocOrThrow(name), x, y);
}
void
ShaderProgram::setUniform(const std::string &name, GLfloat x, GLfloat y,
						  GLfloat z) const {
	glProgramUniform3f(ID, getUniformLocOrThrow(name), x, y, z);
}
void
ShaderProgram::setUniform(const std::string &name, GLfloat x, GLfloat y,
						  GLfloat z, GLfloat w) const {
	glProgramUniform4f(ID, getUniformLocOrThrow(name), x, y, z, w);
}
void
ShaderProgram::setUniform(const std::string &name,
						  const GLfloat mat[4][4]) const {
	glProgramUniformMatrix4fv(ID, getUniformLocOrThrow(name), 1, false,
							  &mat[0][0]);
}
void
ShaderProgram::setUniformSampler(const std::string &name, GLuint texID) const {
	glProgramUniform1i(ID, getUniformLocOrThrow(name),
					   static_cast<GLint>(texID));
}
void
ShaderProgram::saveUniformLocations() {
	GLint numUniforms, uniformMaxLen;
	glGetProgramiv(ID, GL_ACTIVE_UNIFORMS, &numUniforms);
	glGetProgramiv(ID, GL_ACTIVE_UNIFORM_MAX_LENGTH, &uniformMaxLen);
	if (numUniforms <= 0)
		return;
	auto uName = std::make_unique<GLchar[]>(uniformMaxLen);
	GLsizei len;
	GLenum type;
	GLint size;
	for (GLint i = 0; i < numUniforms; ++i) {
		glGetActiveUniform(ID, i, uniformMaxLen, &len, &size, &type,
						   uName.get());
		uniformLocations[std::string(
			uName.get())] = glGetUniformLocation(ID, uName.get());
	}
}
GLint
ShaderProgram::getUniformLocation(const std::string &name) const {
	auto IT = uniformLocations.find(name);
	return IT == uniformLocations.end() ? invalidUniformLoc : IT->second;
}
GLint
ShaderProgram::getUniformLocOrThrow(const std::string &name) const {
	GLint loc = getUniformLocation(name);
	if (loc == invalidUniformLoc)
		throw ShaderException("Uniform \'" + name + "\' could not be found.");
	else
		return loc;
}
} // namespace brdfEditor