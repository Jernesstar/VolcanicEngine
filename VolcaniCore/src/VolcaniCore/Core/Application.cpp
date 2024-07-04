#include "Application.h"

#include <glad/glad.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "Assert.h"

#include "Events/Events.h"

#include "Renderer/Renderer.h"
#include "Renderer/RendererAPI.h"

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
	EventSystem::Init();

	RendererAPI::Create(RendererBackend::OpenGL);
	Renderer::Init();
}

void Application::Close() {
	delete s_Instance;

	Renderer::Close();
	RendererAPI::Close();

	// glfwTerminate();
	exit(0);
}

void Application::Run() {
	while(s_Window->IsOpen()) {
		TimePoint time = Time::GetTime();
		TimeStep ts = time - s_LastFrame;
		s_LastFrame = time;

		EventSystem::PollEvents();
		s_Instance->OnUpdate(ts);

		ApplicationUpdatedEvent event(ts);
		EventSystem::Dispatch(event);

		s_Window->Update();
	}
}

}
