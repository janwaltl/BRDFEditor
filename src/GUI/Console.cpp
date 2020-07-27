#include "Console.hpp"

namespace brdfEditor::gui {
void
Console::printLine(const std::string &line) {
	stream += line;
	stream += '\n';
	newOutput = true;
}

bool
Console::newOutputThisFrame() {
	return newOutput;
}
void
Console::clearNewOutputFlag() {
	newOutput = false;
}
const char *
Console::getBuffer() const {
	return stream.c_str();
}

} // namespace brdfEditor::gui
