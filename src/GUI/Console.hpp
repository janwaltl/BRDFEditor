#ifndef BRDFEDITOR_GUI_CONSOLE_HEADER
#define BRDFEDITOR_GUI_CONSOLE_HEADER

#include <sstream>
#include <string>

namespace brdfEditor::gui {
// Represents a custom console that behaves similary to standard stream but
// is also incorporated into GUI by TwoPanelLayout class.
class Console {
public:
	void
	printLine(const std::string &line);

	const char *
	getBuffer() const;
	// Returns whether any output has been printed since the last
	// clearNewOutputFlag call.
	bool
	newOutputThisFrame();

	void
	clearNewOutputFlag();

private:
	std::string stream;
	bool newOutput = false;
};
} // namespace brdfEditor::gui
#endif // !BRDFEDITOR_GUI_CONSOLE_HEADER
