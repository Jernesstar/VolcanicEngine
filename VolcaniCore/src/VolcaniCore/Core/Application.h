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
				const ImageData& icon = { });
	virtual ~Application() = default;

	static void Close();
	static Application* Get() { return s_Instance; }

	static Ref<Window> GetWindow() { return s_Window; }

	static std::string GetCurrentDir();
	static void PushDir();
	static void PushDir(const std::string& path);
	static void PopDir();

	template<typename TDerived>
	requires std::derived_from<TDerived, Application>
	static TDerived* As() { return (TDerived*)(s_Instance); }

protected:
	virtual void OnUpdate(TimeStep ts) { }

private:
	static void Init();
	static void Run();
	static void SetCurrentDir();

	inline static std::string s_LibraryPath;
	inline static std::string s_Path;

	inline static Application* s_Instance;
	inline static Ref<Window> s_Window;

	inline static TimePoint s_LastFrame{ Time::GetTime() };

	friend int ::main(int argc, char** argv);
};

}