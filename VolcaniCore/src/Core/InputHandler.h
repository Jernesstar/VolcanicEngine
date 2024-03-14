#pragma once

#include <glm/vec2.hpp>

#include "Codes.h"
#include "Defines.h"

namespace VolcaniCore {

class Window;

class InputHandler {
public:
	InputHandler(Window* window) : m_Window(window) { };
	~InputHandler() = default;

	bool KeyPressed(Key key);
	bool KeysPressed(Key key1, Key key2);
	bool KeysPressed(Key key1, Key key2, Key key3);
	bool KeysPressed(Key key1, Key key2, Key key3, Key key4);

	void SetCursorMode(CursorMode mode);
	bool MouseButtonPressed(Mouse mouse_button);

	glm::vec2 GetMousePosition();
	float GetMouseX();
	float GetMouseY();

private:
	Window* m_Window;
};

}