#ifndef BRDFEDITOR_GL_EXCEPTION_HEADER
#define BRDFEDITOR_GL_EXCEPTION_HEADER

#include <string>

#include <glad/glad.h>
#include <src/Core/Exception.hpp>

namespace brdfEditor {
class GLError : public Exception {
public:
	enum error {
		noError = GL_NO_ERROR,
		invalidEnum = GL_INVALID_ENUM,
		invalidValue = GL_INVALID_VALUE,
		invalidOperation = GL_INVALID_OPERATION,
		invalidFBOperation = GL_INVALID_FRAMEBUFFER_OPERATION,
		outOfMemory = GL_OUT_OF_MEMORY,
		stackUnderflow = GL_STACK_UNDERFLOW,
		stackOverflow = GL_STACK_OVERFLOW,
	};
	// Uses default error message for what()
	GLError(error err);
	// Uses custom error msg
	GLError(error err, const std::string &fncName);
	error
	getErrType() {
		return e;
	}

	static GLenum
	errorToGLenum(error e);
	static error
	glEnumToError(GLenum e);

private:
	error e;
};
} // namespace brdfEditor

#endif