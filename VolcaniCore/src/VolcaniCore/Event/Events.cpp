#include "Events.h"

#include <glad/gl.h>

#include "Core/Application.h"
#include "Core/Assert.h"
#include "Core/Input.h"
#include "Core/Log.h"

#define GET_CALLBACKS(TEvent) \
template<> \
Callbacks<TEvent>& Events::GetCallbacks<TEvent>() { \
	return TEvent##Callbacks; \
}

namespace VolcaniCore {

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

void Events::Init() {
	GLFWwindow* window = Application::GetWindow()->GetNativeWindow();

	glfwSetErrorCallback(ErrorCallback);

	glfwSetKeyCallback(window,         KeyCallback);
	glfwSetCharCallback(window,        KeyCharCallback);
	glfwSetCursorPosCallback(window,   MouseMovedCallback);
	glfwSetScrollCallback(window,      MouseScrolledCallback);
	glfwSetMouseButtonCallback(window, MouseButtonCallback);
	glfwSetWindowPosCallback(window,   WindowMovedCallback);
	glfwSetWindowSizeCallback(window,  WindowResizedCallback);
	glfwSetWindowCloseCallback(window, WindowClosedCallback);
}

void Events::ErrorCallback(int error, const char* description) {
	VOLCANICORE_ASSERT(false, description);
}

void Events::KeyCallback(GLFWwindow* window, int key, int scancode,
						 int action, int mods)
{
	if(action == GLFW_PRESS) {
		KeyPressedEvent event((KeyCode)key);
		Dispatch(event);
	}
	else if(action == GLFW_RELEASE) {
		KeyReleasedEvent event((KeyCode)key);
		Dispatch(event);
	}
	else if(action == GLFW_REPEAT) {
		KeyPressedEvent event((KeyCode)key, true);
		Dispatch(event);
	}
}

void Events::KeyCharCallback(GLFWwindow* window, unsigned int codepoint) {
	KeyCharEvent event((KeyCode)codepoint, (char)codepoint);
	Dispatch(event);
}

void Events::MouseMovedCallback(GLFWwindow* window, double x, double y) {
	MouseMovedEvent event((float)x, (float)y);
	Dispatch(event);
}

void Events::MouseScrolledCallback(GLFWwindow* window,
								   double scrollX, double scrollY)
{
	MouseScrolledEvent event((float)scrollX, (float)scrollY);
	Dispatch(event);
}

void Events::MouseButtonCallback(GLFWwindow* window, int button,
								 int action, int mods)
{
	if(action == GLFW_PRESS) {
		MouseButtonPressedEvent event((MouseCode)button,
									  Input::GetMouseX(), Input::GetMouseY());
		Dispatch(event);
	}
	if(action == GLFW_RELEASE) {
		MouseButtonReleasedEvent event((MouseCode)button,
									   Input::GetMouseX(), Input::GetMouseY());
		Dispatch(event);
	}
}

void Events::WindowResizedCallback(GLFWwindow* window, int width, int height) {
	WindowResizedEvent event(width, height);
	Dispatch(event);
}

void Events::WindowMovedCallback(GLFWwindow* window, int x, int y) {
	WindowMovedEvent event(x, y);
	Dispatch(event);
}

void Events::WindowClosedCallback(GLFWwindow* window) {
	WindowClosedEvent event;
	Dispatch(event);
}

}
