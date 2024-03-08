#include "Input.h"

#include "Application.h"

namespace VolcaniCore {

bool Input::KeyPressed(Ref<Window> window, Key key)
{   
    // Note: Get the special cases to work
    // Special cases
    if(key == Key::Ctrl) return KeyPressed(window, Key::LeftCtrl) || KeyPressed(window, Key::RightCtrl);
    if(key == Key::Shift) return KeyPressed(window, Key::LeftShift) || KeyPressed(window, Key::RightShift);
    if(key == Key::Alt) return KeyPressed(window, Key::LeftAlt) || KeyPressed(window, Key::RightAlt);

    auto w = window->GetNativeWindow();
    auto state = glfwGetKey(w, (int)key);

    return state == GLFW_PRESS;
}

void Input::SetCursorMode(Ref<Window> window, CursorMode mode)
{
    auto w = window->GetNativeWindow();
    glfwSetInputMode(w, GLFW_CURSOR, GLFW_CURSOR_NORMAL + (int)mode);
}

bool Input::MouseButtonPressed(Ref<Window> window, Mouse mouse_button)
{
    auto w = window->GetNativeWindow();
    auto state = glfwGetMouseButton(w, (int)(mouse_button));

    return state == GLFW_PRESS;
}

glm::vec2 Input::GetMousePosition(Ref<Window> window)
{
    auto w = window->GetNativeWindow();
    double x, y;
    glfwGetCursorPos(w, &x, &y);

    return { (float)x, (float)y };
}

float Input::GetMouseX(Ref<Window> window) { return GetMousePosition(window).x; }
float Input::GetMouseY(Ref<Window> window) { return GetMousePosition(window).y; }

bool Input::KeysPressed(Ref<Window> window, Key key1, Key key2)
{
    if(KeyPressed(window, key1))
        if(KeyPressed(window, key2)) return true;
    return false;
}

bool Input::KeysPressed(Ref<Window> window, Key key1, Key key2, Key key3)
{
    if(KeyPressed(window, key1))
        if(KeyPressed(window, key2))
            if(KeyPressed(window, key3)) return true;
    return false;
}

bool Input::KeysPressed(Ref<Window> window, Key key1, Key key2, Key key3, Key key4)
{
    if(KeyPressed(window, key1))
        if(KeyPressed(window, key2))
            if(KeyPressed(window, key3))
                if(KeyPressed(window, key4)) return true;
    return false;
}

}