#pragma once

#include <glm/vec2.hpp>

#include "Codes.h"

namespace VolcaniCore {

class Input {
public:
    static bool KeyPressed(Key key);
    static bool KeysPressed(Key key1, Key key2);
    static bool KeysPressed(Key key1, Key key2, Key key3);
    static bool KeysPressed(Key key1, Key key2, Key key3, Key key4);

    static void SetCursorMode(CursorMode mode);
    static bool MouseButtonPressed(Mouse mouse_button);

    static glm::vec2 GetMousePosition();
    static float GetMouseX();
    static float GetMouseY();

private:
    Input() = delete;
    ~Input() = delete;
};

}