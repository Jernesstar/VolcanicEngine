#pragma once

#include <glm/vec2.hpp>

#include "Codes.h"
#include "Window.h"

namespace VolcaniCore {

class Input {
public:
    static bool KeyPressed(Ref<Window> window, Key key);
    static bool KeysPressed(Ref<Window> window, Key key1, Key key2);
    static bool KeysPressed(Ref<Window> window, Key key1, Key key2, Key key3);
    static bool KeysPressed(Ref<Window> window, Key key1, Key key2, Key key3, Key key4);

    static void SetCursorMode(Ref<Window> window, CursorMode mode);
    static bool MouseButtonPressed(Ref<Window> window, Mouse mouse_button);

    static glm::vec2 GetMousePosition(Ref<Window> window);
    static float GetMouseX(Ref<Window> window);
    static float GetMouseY(Ref<Window> window);

private:
    Input() = delete;
    ~Input() = delete;
};

}