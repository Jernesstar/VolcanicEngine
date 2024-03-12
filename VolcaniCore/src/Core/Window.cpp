#include "Window.h"

#include "Assert.h"

namespace VolcaniCore {

Window::Window(uint32_t width, uint32_t height)
{
	glfwWindowHint(GLFW_SAMPLES, 4);

	m_NativeWindow = glfwCreateWindow(width, height, "Window", nullptr, nullptr);
	VOLCANICORE_ASSERT(m_NativeWindow, "Could not create the window");
	glfwMakeContextCurrent(m_NativeWindow);

	m_InputHandler = new InputHandler(this);
}

Window::~Window()
{
    if(m_NativeWindow) glfwDestroyWindow(m_NativeWindow);
    m_NativeWindow = nullptr;
}

}