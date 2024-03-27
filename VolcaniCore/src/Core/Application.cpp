#include "Application.h"

#include <glad/glad.h>

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

#include "Assert.h"
#include "Time.h"
#include "Renderer/Renderer.h"
#include "Events/Events.h"

namespace VolcaniCore {

Application::Application()
	: m_Window(CreateRef<Window>(800, 600))
{
	VOLCANICORE_ASSERT(gladLoadGL(), "Glad could not load OpenGL");

	s_Instance = this;

	EventSystem::Init();
	Renderer::Init(RenderAPI::OpenGL);
}

void Application::Init() {
	VOLCANICORE_ASSERT(glfwInit(), "Failed to initialize GLFW");
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