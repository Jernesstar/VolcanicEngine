#pragma once

#include "Time.h"
#include "Defines.h"
#include "Window.h"

int main(int argc, char** argv);

namespace VolcaniCore {

class Application {
public:
	Application(uint32_t width = 800, uint32_t height = 600,
				const std::string& title = "Application",
				const std::string& iconPath = "");
	virtual ~Application() = default;

	static void Close();

	static Ref<Window> GetWindow() { return s_Window; }

protected:
	virtual void OnUpdate(TimeStep ts) = 0;

private:
	static void Init();
	static void Run();

	inline static Application* s_Instance;
	inline static Ref<Window> s_Window;

	inline static TimePoint s_LastFrame{ Time::GetTime() };
	inline static TimeStep s_TimeStep;

	friend int ::main(int argc, char** argv);
};

}