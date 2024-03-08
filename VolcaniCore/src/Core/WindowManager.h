#pragma once

#include <vector>

#include "Defines.h"
#include "Window.h"

namespace VolcaniCore {

class WindowManager {
public:
    Ref<Window> CreateWindow();
    Ref<Window> GetWindow(uint32_t index);
    void CloseWindow(uint32_t index);

private:
    std::vector<Ref<Window>> m_Windows;
};


}