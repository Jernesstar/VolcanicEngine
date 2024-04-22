#include "Application.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#if defined(VOLCANICENGINE_LINUX)
	#define GLFW_EXPOSE_NATIVE_X11
#elif defined(VOLCANICENGINE_WINDOWS)
	#define GLFW_EXPOSE_NATIVE_WIN32
#endif // VOLCANICENGINE_LINUX

#include <GLFW/glfw3native.h>

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

#include "Assert.h"
#include "Time.h"
#include "Renderer/Renderer.h"
#include "Events/Events.h"

namespace VolcaniCore {

Application::Application()
	: m_Window(CreateRef<Window>(800, 600))
{
	bgfx::PlatformData pd;
#if defined(VOLCANICENGINE_LINUX)
	pd.nwh = (void*)glfwGetX11Window(m_Window->GetNativeWindow());
#elif defined(VOLCANICENGINE_WINDOWS)
	pd.nwh = (void*)glfwGetWin32Window(m_Window->GetNativeWindow());
#endif // VOLCANICENGINE_LINUX

	bgfx::setPlatformData(pd);

	bgfx::Init bgfxInit;
	bgfxInit.type = bgfx::RendererType::Count; // Automatically choose a renderer.
	bgfxInit.resolution.width = m_Window->m_Width;
	bgfxInit.resolution.height = m_Window->m_Height;
	bgfxInit.resolution.reset = BGFX_RESET_VSYNC;
	bgfx::init(bgfxInit);

	s_Instance = this;
	EventSystem::Init();
	Renderer::Init(RenderAPI::OpenGL);
}

void Application::Init() {
	VOLCANICORE_ASSERT(glfwInit(), "Failed to initialize GLFW");
	VOLCANICORE_ASSERT(bgfx::init(), "Failed to initialize GLFW");
}

void Application::Run() {
	while(s_Instance->m_Window->IsOpen())
	{
		TimePoint time = Time::GetTime();
		TimeStep ts = time - s_LastFrame;
		s_LastFrame = time;
		s_TimeStep = ts;

		ApplicationUpdatedEvent event(ts);
		EventSystem::Dispatch(event);
		EventSystem::PollEvents();
		
		s_Instance->OnUpdate(ts);
		s_Instance->m_Window->Update();
	}	
}

void Application::Close() {
	delete s_Instance;
	glfwTerminate();
	exit(0);
}

}