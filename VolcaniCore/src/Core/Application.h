#pragma once

#include "WindowManager.h"

namespace VolcaniCore {

class Application {
public:
	Application();

	static void Init();
	static void Run() { while(true) { } };
	static void Close();

private:
	WindowManager m_WindowManager;
};


}