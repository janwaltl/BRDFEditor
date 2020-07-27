#include "GLException.hpp"

#include <cassert>

#define GL_ERROR_STRINGIFY(X) #X

namespace brdfEditor {
namespace {
std::string
translateError(GLError::error e) {
	switch (e) {
	case brdfEditor::GLError::noError:
		return "No error";
	case brdfEditor::GLError::invalidEnum:
		return GL_ERROR_STRINGIFY(GL_INVALID_ENUM);
	case brdfEditor::GLError::invalidValue:
		return GL_ERROR_STRINGIFY(GL_INVALID_VALUE);
	case brdfEditor::GLError::invalidOperation:
		return GL_ERROR_STRINGIFY(GL_INVALID_OPERATION);
	case brdfEditor::GLError::invalidFBOperation:
		return GL_ERROR_STRINGIFY(GL_INVALID_FRAMEBUFFER_OPERATION);
	case brdfEditor::GLError::outOfMemory:
		return GL_ERROR_STRINGIFY(GL_OUT_OF_MEMORY);
	case brdfEditor::GLError::stackUnderflow:
		return GL_ERROR_STRINGIFY(GL_STACK_UNDERFLOW);
	case brdfEditor::GLError::stackOverflow:
		return GL_ERROR_STRINGIFY(GL_STACK_OVERFLOW);
	default:
		assert("Forgot to add enum");
		return "UKNOWN ERROR";
	}
}
std::string
makeDefErrMsg(GLError::error e) {
	return "OpenGL encountered an error: " + translateError(e);
}
std::string
makeFncErrMsg(const std::string &fncName, GLError::error e) {
	return "OpenGL function '" + fncName +
		   "' encountered an error: " + translateError(e);
}
} // namespace
GLError::GLError(GLError::error err) : Exception(makeDefErrMsg(err)), e(err) {}

GLError::GLError(GLError::error err, const std::string &fncName) :
	Exception(makeFncErrMsg(fncName, err)),
	e(err) {}

GLenum
GLError::errorToGLenum(GLError::error e) {
	switch (e) {
	case GLError::noError:
		return GL_NO_ERROR;
	case GLError::invalidEnum:
		return GL_INVALID_ENUM;
	case GLError::invalidValue:
		return GL_INVALID_VALUE;
	case GLError::invalidOperation:
		return GL_INVALID_OPERATION;
	case GLError::invalidFBOperation:
		return GL_INVALID_FRAMEBUFFER_OPERATION;
	case GLError::outOfMemory:
		return GL_OUT_OF_MEMORY;
	case GLError::stackUnderflow:
		return GL_STACK_UNDERFLOW;
	case GLError::stackOverflow:
		return GL_STACK_OVERFLOW;
	default:
		assert(0);
		return GL_NO_ERROR;
	}
}
GLError::error
GLError::glEnumToError(GLenum e) {
	switch (e) {
	case GL_NO_ERROR:
		return GLError::noError;
	case GL_INVALID_ENUM:
		return GLError::invalidEnum;
	case GL_INVALID_VALUE:
		return GLError::invalidValue;
	case GL_INVALID_OPERATION:
		return GLError::invalidOperation;
	case GL_INVALID_FRAMEBUFFER_OPERATION:
		return GLError::invalidFBOperation;
	case GL_OUT_OF_MEMORY:
		return GLError::outOfMemory;
	case GL_STACK_UNDERFLOW:
		return GLError::stackUnderflow;
	case GL_STACK_OVERFLOW:
		return GLError::stackOverflow;
	default:
		assert(0);
		return GLError::noError;
	}
}
} // namespace brdfEditor