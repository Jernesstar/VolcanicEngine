#include "InputHandler.h"

#include "Window.h"

namespace VolcaniCore {

bool InputHandler::KeyPressed(Key key)
{   
    // Note: Get the special cases to work
    // Special cases
    if(key == Key::Ctrl) return KeyPressed(Key::LeftCtrl) || KeyPressed(Key::RightCtrl);
    if(key == Key::Shift) return KeyPressed(Key::LeftShift) || KeyPressed(Key::RightShift);
    if(key == Key::Alt) return KeyPressed(Key::LeftAlt) || KeyPressed(Key::RightAlt);

    auto w = m_Window->GetNativeWindow();
    auto state = glfwGetKey(w, (int)key);

    return state == GLFW_PRESS;
}

void InputHandler::SetCursorMode(CursorMode mode)
{
    auto w = m_Window->GetNativeWindow();
    glfwSetInputMode(w, GLFW_CURSOR, GLFW_CURSOR_NORMAL + (int)mode);
}

bool InputHandler::MouseButtonPressed(Mouse mouse_button)
{
    auto w = m_Window->GetNativeWindow();
    auto state = glfwGetMouseButton(w, (int)(mouse_button));

    return state == GLFW_PRESS;
}

glm::vec2 InputHandler::GetMousePosition() {
    auto w = m_Window->GetNativeWindow();
    double x, y;
    glfwGetCursorPos(w, &x, &y);

    return { (float)x, (float)y };
}

float InputHandler::GetMouseX() { return GetMousePosition().x; }
float InputHandler::GetMouseY() { return GetMousePosition().y; }

bool InputHandler::KeysPressed(Key key1, Key key2)
{
    if(KeyPressed(key1))
        if(KeyPressed(key2)) return true;
    return false;
}

bool InputHandler::KeysPressed(Key key1, Key key2, Key key3)
{
    if(KeyPressed(key1))
        if(KeyPressed(key2))
            if(KeyPressed(key3)) return true;
    return false;
}

bool InputHandler::KeysPressed(Key key1, Key key2, Key key3, Key key4)
{
    if(KeyPressed(key1))
        if(KeyPressed(key2))
            if(KeyPressed(key3))
                if(KeyPressed(key4)) return true;
    return false;
}

}