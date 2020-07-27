#ifndef BRDFEDITOR_INPUT_HEADER
#define BRDFEDITOR_INPUT_HEADER

#include <array>
#include <cstddef>

#include "PassKey.hpp"

namespace brdfEditor {
class WindowManager;

// Collects keyboard & mouse input for a given GLFWwindow.
class Input {
public:
	Input();
	using Owner = WindowManager;
	enum state { press, repeat, release };
	enum mouseB {
		left,
		middle,
		right,
	};
	// TODO Replace with a vector?(glm?)
	struct mouseCoords {
		double x, y;
	};
	state
	getKey(std::size_t keyIndex);
	void
	setKey(std::size_t keyIndex, state newState, PassKey<Owner>);
	state
	getMouseButton(mouseB button);
	void
	setMouseButton(mouseB button, state newState, PassKey<Owner>);
	mouseCoords
	getMousePos();
	void
	setMousePos(mouseCoords newScroll, PassKey<Owner>);
	mouseCoords
	getMouseScroll();
	void
	setMouseScroll(mouseCoords newScroll, PassKey<Owner>);

private:
	// Stores keys' states
	// 1024 is enough as GLFW's current max index is under 400
	std::array<state, 512> keys;
	struct {
		state left, middle, right;
	} mouseButtons;
	mouseCoords mouseScroll;
	mouseCoords mousePos;
};
} // namespace brdfEditor
#endif // !BRDFEDITOR_INPUT_HEADER
