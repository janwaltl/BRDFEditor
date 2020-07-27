#include "Shader.hpp"

#include <cassert>
#include <string>
#include <memory>

#include "ShaderException.hpp"

namespace brdfEditor {
namespace {
GLenum
ShaderTypeToGLEnum(Shader::type t) {
	switch (t) {
	case brdfEditor::Shader::type::vertex:
		return GL_VERTEX_SHADER;
		break;
	case brdfEditor::Shader::type::fragment:
		return GL_FRAGMENT_SHADER;
		break;
	case brdfEditor::Shader::type::geometry:
		return GL_GEOMETRY_SHADER;
		break;
	case brdfEditor::Shader::type::compute:
		return GL_COMPUTE_SHADER;
		break;
	default:
		assert("Forgot an enum option");
		return GL_FALSE;
		break;
		break;
	}
}
} // namespace
Shader::Shader(type t, const std::string &source) {
	ID = glCreateShader(ShaderTypeToGLEnum(t));
	if (ID == 0)
		throw ShaderException("Could not create glShader object.");
	const GLchar *shaderSource = source.c_str();
	glShaderSource(ID, 1, &shaderSource, nullptr);

	glCompileShader(ID);

	GLint status;
	glGetShaderiv(ID, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE) {
		GLint length;
		glGetShaderiv(ID, GL_INFO_LOG_LENGTH, &length);
		auto log = std::make_unique<GLchar[]>(length);
		glGetShaderInfoLog(ID, length, nullptr, log.get());
		throw ShaderException("Shader compilation has failed, log:\n" +
							  std::string(log.get()));
	}
}
Shader::~Shader() { glDeleteShader(ID); }
void
Shader::attachTo(GLint program) const {
	glAttachShader(program, this->ID);
}
void
Shader::detachFrom(GLint program) const {
	glDetachShader(program, this->ID);
}
} // namespace brdfEditor