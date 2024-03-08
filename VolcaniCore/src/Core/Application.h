#pragma once

#include "Window.h"

namespace VolcaniCore {

class Application {
public:
	Application();

	static void Init();

private:
	Window m_Window;

	Renderer m_Renderer;
};


}