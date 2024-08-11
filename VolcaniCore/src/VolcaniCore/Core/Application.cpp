#include "Application.h"

#include "Assert.h"

#include "Event/Events.h"

#include "Renderer/Renderer.h"
#include "Renderer/RendererAPI.h"
#include "Renderer/ShaderLibrary.h"

namespace VolcaniCore {

Application::Application(uint32_t width, uint32_t height,
						 const std::string& title, const std::string& iconPath)
{
	s_Window->SetIcon(iconPath);
	s_Window->SetTitle(title);
	s_Window->Resize(width, height);
	s_Instance = this;
}

void Application::Init() {
	VOLCANICORE_ASSERT(glfwInit(), "Failed to initialize GLFW");

	s_Window = CreateRef<Window>(800, 600);
	Events::Init();

	RendererAPI::Create(RendererAPI::Backend::OpenGL);
	Renderer::Init();
}

void Application::Close() {
	delete s_Instance;

	Renderer::Close();
	RendererAPI::Shutdown();

	// glfwTerminate();
	exit(0);
}

void Application::Run() {
	while(s_Window->IsOpen()) {
		TimePoint time = Time::GetTime();
		TimeStep ts = time - s_LastFrame;
		s_LastFrame = time;

		Events::PollEvents();

		Renderer::BeginFrame();
		{
			s_Instance->OnUpdate(ts);
			// s_Instance->OnRender();
		}
		Renderer::EndFrame();

		s_Window->Update();
	}
}

}
