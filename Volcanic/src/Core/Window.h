#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "Defines.h"

namespace Volcanic {

class Window {
public:
    Window();

private:
    GLFWwindow* m_Window;
};


}