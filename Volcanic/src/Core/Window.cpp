#include "Window.h"

namespace Volcanic {

Window::Window()
{
    glfwWindowHint(GLFW_SAMPLES, 4);

    m_Window = glfwCreateWindow(specs.Width, specs.Height, specs.Title.c_str(), nullptr, nullptr);
    SADDLE_CORE_ASSERT(m_Window, "Could not create the window");

}


}