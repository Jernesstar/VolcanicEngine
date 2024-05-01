#pragma once

#include "Defines.h"
#include "Window.h"
#include "Time.h"

int main(int argc, char** argv);

namespace VolcaniCore {

class Application {
public:
	Application();
	virtual ~Application() = default;

	static void Close();

	static Ref<Window> GetWindow() { return s_Window; }
	static Ref<APIRenderer> GetBackendRenderer() { return s_BackendRenderer; }

protected:
	virtual void OnUpdate(TimeStep ts) = 0;

private:
	static void Init();
	static void Run();

	inline static Application* s_Instance;
	inline static Ref<Window> s_Window;
	inline static Ref<APIRenderer> s_BackendRenderer;

	inline static TimePoint s_LastFrame{ Time::GetTime() };
	inline static TimeStep s_TimeStep;

	friend int ::main(int argc, char** argv);
	friend class Window;
};

}
