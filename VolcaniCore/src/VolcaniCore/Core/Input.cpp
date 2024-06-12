#include "Input.h"

#include "Application.h"

namespace VolcaniCore {

bool Input::KeyPressed(Key key)
{   
	// TODO: Get the special cases to work
	if(key == Key::Ctrl) return KeyPressed(Key::LeftCtrl) || KeyPressed(Key::RightCtrl);
	if(key == Key::Shift) return KeyPressed(Key::LeftShift) || KeyPressed(Key::RightShift);
	if(key == Key::Alt) return KeyPressed(Key::LeftAlt) || KeyPressed(Key::RightAlt);

	auto window = Application::GetWindow()->GetNativeWindow();
	auto state = glfwGetKey(window, (int)key);

	return state == GLFW_PRESS;
}

void Input::SetCursorMode(CursorMode mode)
{
	GLFWwindow* window = Application::GetWindow()->GetNativeWindow();
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL + (int)mode);
}

bool Input::MouseButtonPressed(Mouse mouse_button)
{
	auto window = Application::GetWindow()->GetNativeWindow();
	auto state = glfwGetMouseButton(window, (int)(mouse_button));

	return state == GLFW_PRESS;
}

glm::vec2 Input::GetMousePosition()
{
	auto window = Application::GetWindow()->GetNativeWindow();
	double x, y;
	glfwGetCursorPos(window, &x, &y);

	return { (float)x, (float)y };
}

float Input::GetMouseX() { return GetMousePosition().x; }
float Input::GetMouseY() { return GetMousePosition().y; }

bool Input::KeysPressed(Key key1, Key key2)
{
	if(KeyPressed(key1))
		if(KeyPressed(key2)) return true;
	return false;
}

bool Input::KeysPressed(Key key1, Key key2, Key key3)
{
	if(KeyPressed(key1))
		if(KeyPressed(key2))
			if(KeyPressed(key3)) return true;
	return false;
}

bool Input::KeysPressed(Key key1, Key key2, Key key3, Key key4)
{
	if(KeyPressed(key1))
		if(KeyPressed(key2))
			if(KeyPressed(key3))
				if(KeyPressed(key4)) return true;
	return false;
}

}