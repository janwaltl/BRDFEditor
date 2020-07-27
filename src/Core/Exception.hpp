#ifndef BRDFEDITOR_EXCEPTION_HEADER
#define BRDFEDITOR_EXCEPTION_HEADER

#include <stdexcept>

namespace brdfEditor {
class Exception : public std::logic_error {
	using std::logic_error::logic_error;
};
} // namespace brdfEditor
#endif // ! BRDFEDITOR_EXCEPTION_HEADER
