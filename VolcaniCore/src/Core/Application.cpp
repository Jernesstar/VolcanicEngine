#include "Application.h"

#include <glad/glad.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "Assert.h"
#include "Time.h"
#include "Events/Events.h"
#include "Renderer/Renderer.h"

namespace VolcaniCore {

Application::Application() {
	s_Instance = this;
}

void Application::Init() {
	VOLCANICORE_ASSERT(glfwInit(), "Failed to initialize GLFW");

	s_Window = CreateRef<Window>(800, 600);

	s_BackendRenderer = RendererAPI::CreateRenderer(RenderAPI::OpenGL);
	s_BackendRenderer->Init();

	Renderer::Init();
	EventSystem::Init();
}

void Application::Close() {
	delete s_Instance;
	glfwTerminate();
	exit(0);
}

void Application::Run() {
	while(s_Window->IsOpen()) {
		TimePoint time = Time::GetTime();
		TimeStep ts = time - s_LastFrame;
		s_LastFrame = time;
		s_TimeStep = ts;

		ApplicationUpdatedEvent event(ts);
		EventSystem::Dispatch(event);
		EventSystem::PollEvents();

		s_Instance->OnUpdate(ts);
		s_Window->Update();
	}
}

}