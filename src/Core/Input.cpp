#include "Input.hpp"

#include <cassert>

namespace brdfEditor {
Input::Input() {
	keys.fill(release);
	mouseButtons = {release, release, release};
	mousePos = {0.0, 0.0};
}
Input::state
Input::getKey(std::size_t keyIndex) {
	assert(keyIndex < keys.size());
	return keys[keyIndex];
}
void
Input::setKey(std::size_t keyIndex, Input::state newState,
			  PassKey<Input::Owner>) {
	assert(keyIndex < keys.size());
	keys[keyIndex] = newState;
}
Input::state
Input::getMouseButton(Input::mouseB button) {
	switch (button) {
	case brdfEditor::Input::left:
		return mouseButtons.left;
	case brdfEditor::Input::middle:
		return mouseButtons.middle;
	case brdfEditor::Input::right:
		return mouseButtons.right;
	default:
		assert("Forgot to add an enum");
		return release;
	}
}
void
Input::setMouseButton(Input::mouseB button, state newState,
					  PassKey<Input::Owner>) {
	switch (button) {
	case brdfEditor::Input::left:
		mouseButtons.left = newState;
	case brdfEditor::Input::middle:
		mouseButtons.middle = newState;
	case brdfEditor::Input::right:
		mouseButtons.right = newState;
	default:
		assert("Forgot to add an enum");
	}
}
Input::mouseCoords
Input::getMousePos() {
	return mousePos;
}
void
Input::setMousePos(Input::mouseCoords newPos, PassKey<Input::Owner>) {
	mousePos = newPos;
}
Input::mouseCoords
Input::getMouseScroll() {
	return mouseScroll;
}
void
Input::setMouseScroll(Input::mouseCoords newScroll, PassKey<Input::Owner>) {
	mouseScroll = newScroll;
}
} // namespace brdfEditor