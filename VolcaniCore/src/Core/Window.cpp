#include "Window.h"

#include "Assert.h"

namespace VolcaniCore {

Window::Window(uint32_t width, uint32_t height)
{
	glfwWindowHint(GLFW_SAMPLES, 4);

	m_Window = glfwCreateWindow(width, height, "Window", nullptr, nullptr);
	VOLCANICORE_ASSERT(m_Window, "Could not create the window");
	glfwMakeContextCurrent(m_Window);

	while(true) {
		Update();
	}
}

Window::~Window()
{
    if(m_Window) glfwDestroyWindow(m_Window);
    m_Window = nullptr;
}

}