 #pragma once

#include "Defines.h"
#include "Window.h"
#include "Time.h"

#include "Renderer/RendererAPI.h"

int main(int argc, char** argv);

namespace VolcaniCore {

class Application {
public:
	Application(uint32_t width = 800, uint32_t height = 600,
		const std::string_view& title = "",
		const std::string_view& iconPath = "");
	virtual ~Application() = default;

	static void Close();

	static Ref<Window> GetWindow() { return s_Window; }
	static Ref<RendererAPI> GetRenderer() { return s_BackendRenderer; }
	static RenderAPI GetRenderAPI() { return s_BackendRenderer->API; }

protected:
	virtual void OnUpdate(TimeStep ts) = 0;

private:
	static void Init();
	static void Run();

	inline static Application* s_Instance;
	inline static Ref<Window> s_Window;
	inline static Ref<RendererAPI> s_BackendRenderer;

	inline static TimePoint s_LastFrame{ Time::GetTime() };
	inline static TimeStep s_TimeStep;

	friend int ::main(int argc, char** argv);
	friend class Window;
};

}