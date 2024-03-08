#include "EventSystem.h"

#include <glad/glad.h>

#include "Core/Application.h"
#include "Core/Assert.h"
#include "Core/Input.h"
#include "Core/Log.h"


#define GET_CALLBACKS(TEvent) \
template<> \
Callbacks<TEvent>& EventSystem::GetCallbacks<TEvent>() \
{ \
    return TEvent##Callbacks; \
}

namespace VolcaniCore {

void EventSystem::Init(Ref<Window> window)
{
    auto w = window->GetNativeWindow();
    VOLCANICORE_ASSERT(window);

    glfwSetErrorCallback(ErrorCallback);
    glfwSetKeyCallback(w, KeyCallback);
    glfwSetCharCallback(w, KeyCharCallback);
    glfwSetCursorPosCallback(w, MouseMovedCallback);
    glfwSetScrollCallback(w, MouseScrolledCallback);
    glfwSetMouseButtonCallback(w, MouseButtonCallback);
    glfwSetWindowPosCallback(w, WindowMovedCallback);
    glfwSetWindowSizeCallback(w, WindowResizedCallback);
    glfwSetWindowCloseCallback(w, WindowClosedCallback);
}

GET_CALLBACKS(KeyPressedEvent);
GET_CALLBACKS(KeyReleasedEvent);
GET_CALLBACKS(KeyCharEvent);
GET_CALLBACKS(MouseMovedEvent);
GET_CALLBACKS(MouseScrolledEvent);
GET_CALLBACKS(MouseButtonPressedEvent);
GET_CALLBACKS(MouseButtonReleasedEvent);
GET_CALLBACKS(WindowResizedEvent);
GET_CALLBACKS(WindowMovedEvent);
GET_CALLBACKS(WindowClosedEvent);
GET_CALLBACKS(ApplicationUpdatedEvent);

template<>
EventCallback<KeyEvent> EventSystem::RegisterEventListener<KeyEvent>(const std::function<void(KeyEvent&)>& event_callback)
{
    EventCallback<KeyEvent> _event_callback(event_callback);
    RegisterEventListener<KeyPressedEvent>((EventCallback<KeyEvent>)_event_callback);
    RegisterEventListener<KeyReleasedEvent>((EventCallback<KeyEvent>)_event_callback);
    return _event_callback;
}

template<>
void EventSystem::RegisterEventListener<KeyEvent>(const EventCallback<KeyEvent>& event_callback)
{
    RegisterEventListener<KeyPressedEvent>((EventCallback<KeyEvent>)event_callback);
    RegisterEventListener<KeyReleasedEvent>((EventCallback<KeyEvent>)event_callback);
}

template<>
EventCallback<MouseEvent> EventSystem::RegisterEventListener<MouseEvent>(const std::function<void(MouseEvent&)>& event_callback)
{
    EventCallback<MouseEvent> _event_callback(event_callback);
    RegisterEventListener<MouseMovedEvent>((EventCallback<MouseMovedEvent>)_event_callback);
    RegisterEventListener<MouseScrolledEvent>((EventCallback<MouseScrolledEvent>)_event_callback);
    RegisterEventListener<MouseButtonPressedEvent>((EventCallback<MouseButtonPressedEvent>)_event_callback);
    RegisterEventListener<MouseButtonReleasedEvent>((EventCallback<MouseButtonReleasedEvent>)_event_callback);
    return _event_callback;
}

template<>
void EventSystem::RegisterEventListener<MouseEvent>(const EventCallback<MouseEvent>& event_callback)
{
    RegisterEventListener<MouseMovedEvent>((EventCallback<MouseMovedEvent>)event_callback);
    RegisterEventListener<MouseScrolledEvent>((EventCallback<MouseScrolledEvent>)event_callback);
    RegisterEventListener<MouseButtonPressedEvent>((EventCallback<MouseButtonPressedEvent>)event_callback);
    RegisterEventListener<MouseButtonReleasedEvent>((EventCallback<MouseButtonReleasedEvent>)event_callback);
}

template<>
EventCallback<WindowEvent> EventSystem::RegisterEventListener<WindowEvent>(const std::function<void(WindowEvent&)>& event_callback)
{
    EventCallback<WindowEvent> _event_callback(event_callback);
    RegisterEventListener<WindowResizedEvent>((EventCallback<WindowResizedEvent>)_event_callback);
    RegisterEventListener<WindowClosedEvent>((EventCallback<WindowClosedEvent>)_event_callback);
    return _event_callback;
}

template<>
void EventSystem::RegisterEventListener<WindowEvent>(const EventCallback<WindowEvent>& event_callback)
{
    RegisterEventListener<WindowResizedEvent>((EventCallback<WindowResizedEvent>)event_callback);
    RegisterEventListener<WindowClosedEvent>((EventCallback<WindowClosedEvent>)event_callback);
}

template<>
EventCallback<Event> EventSystem::RegisterEventListener<Event>(const std::function<void(Event&)>& event_callback)
{
    EventCallback<Event> _event_callback(event_callback);
    RegisterEventListener<KeyEvent>((EventCallback<KeyEvent>)_event_callback);
    RegisterEventListener<MouseEvent>((EventCallback<MouseEvent>)_event_callback);
    RegisterEventListener<WindowEvent>((EventCallback<WindowEvent>)_event_callback);
    return _event_callback;
}

template<>
void EventSystem::RegisterEventListener<Event>(const EventCallback<Event>& event_callback)
{
    RegisterEventListener<KeyEvent>((EventCallback<KeyEvent>)event_callback);
    RegisterEventListener<MouseEvent>((EventCallback<MouseEvent>)event_callback);
    RegisterEventListener<WindowEvent>((EventCallback<WindowEvent>)event_callback);
}

void EventSystem::ErrorCallback(int error, const char* description)
{
    VOLCANICORE_ASSERT(false, description);
}

void EventSystem::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(action == GLFW_PRESS)
    {
        KeyPressedEvent event((KeyCode)key);
        Dispatch(event);
    }

    if(action == GLFW_RELEASE)
    {
        KeyReleasedEvent event((KeyCode)key);
        Dispatch(event);
    }

    if(action == GLFW_REPEAT)
    {
        KeyPressedEvent event((KeyCode)key, true);
        Dispatch(event);
    }
}

void EventSystem::KeyCharCallback(GLFWwindow* window, unsigned int codepoint)
{
    KeyCharEvent event((KeyCode)codepoint, (char)codepoint);
    Dispatch(event);
}

void EventSystem::MouseMovedCallback(GLFWwindow* window, double x, double y)
{
    MouseMovedEvent event((float)x, (float)y);
    Dispatch(event);
}

void EventSystem::MouseScrolledCallback(GLFWwindow* window, double x_scroll, double y_scroll)
{
    MouseScrolledEvent event((float)x_scroll, (float)y_scroll);
    Dispatch(event);
}

void EventSystem::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    // if(action == GLFW_PRESS)
    // {
    //     MouseButtonPressedEvent event((MouseCode)button, Input::GetMouseX(), Input::GetMouseY());
    //     Dispatch(event);
    // }

    // if(action == GLFW_RELEASE)
    // {
    //     MouseButtonReleasedEvent event((MouseCode)button, Input::GetMouseX(), Input::GetMouseY());
    //     Dispatch(event);
    // }
}

void EventSystem::WindowResizedCallback(GLFWwindow* window, int width, int height)
{
    WindowResizedEvent event(width, height);
    Dispatch(event);
}

void EventSystem::WindowMovedCallback(GLFWwindow* window, int x, int y)
{
    WindowMovedEvent event(x, y);
    Dispatch(event);
}
void EventSystem::WindowClosedCallback(GLFWwindow* window)
{
    WindowClosedEvent event;
    Dispatch(event);
}

}
