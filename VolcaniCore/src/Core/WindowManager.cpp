#include "WindowManager.h"

namespace VolcaniCore {

Ref<Window> WindowManager::CreateWindow(unint32_t width, uint32_t height) {
    auto window = CreateRef<Window>(width, height);
    m_Windows.push_back(window);
    return window;
}

}