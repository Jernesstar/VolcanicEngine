#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "Defines.h"

namespace VolcaniCore {

class Window {
public:
    Window();
    Window(uint32_t width, uint32_t height);

private:
    uint32_t m_Width, m_Height;
    GLFWwindow* m_Window;
};


}