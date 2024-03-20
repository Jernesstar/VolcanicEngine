#pragma once

#include "Defines.h"
#include "Window.h"
#include "Time.h"

namespace VolcaniCore {

class Application {
public:
	Application();

	static void Init();
	static void Run();
	static void Close();

	static Application* Get() { return s_Instance; }

	Ref<Window> GetWindow() { return m_Window; }

private:
	Ref<Window> m_Window;
	inline static Application* s_Instance;
    inline static TimePoint s_LastFrame{ Time::GetTime() };
    inline static TimeStep s_TimeStep;
};


}