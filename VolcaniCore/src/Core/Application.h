#pragma once

#include "Defines.h"
#include "Window.h"
#include "Time.h"

int main(int argc, char** argv);

namespace VolcaniCore {

class Application {
public:
	Application();

	static void Close();
	static Ref<Window> GetWindow() { return s_Instance->m_Window; }

protected:
	virtual ~Application() = default;

	virtual void OnUpdate(TimeStep ts) = 0;
	Ref<Window> m_Window;

private:
	static void Init();
	static void Run();

	inline static Application* s_Instance;
	inline static TimePoint s_LastFrame{ Time::GetTime() };
	inline static TimeStep s_TimeStep;

	friend int ::main(int argc, char** argv);
	friend class Window;
};

}
