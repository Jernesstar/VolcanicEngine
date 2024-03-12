#pragma once

#include <vector>

#include "Defines.h"
#include "Window.h"

namespace VolcaniCore {

class WindowManager {
public:
	WindowManager() = default;
	~WindowManager() = default;

	uint32_t CreateWindow(uint32_t width, uint32_t height);
	Ref<Window> GetWindow(uint32_t index);

	void CloseWindow(uint32_t index);

private:
	std::vector<Ref<Window>> m_Windows;
	uint32_t index = -1;
};


}