#include "Application.h"
#include "Events/Events.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "Assert.h"
#include "Time.h"
#include "Renderer/Renderer.h"

namespace VolcaniCore {

Application::Application() {
	s_Instance = this;
	EventSystem::Init();
	Renderer::Init();
}

void Application::Init() {
	s_Window = CreateRef<Window>(800, 600);
	VOLCANICORE_ASSERT(glfwInit(), "Failed to initialize GLFW");
}

void Application::Run() {
	while(s_Window->IsOpen())
	{
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

void Application::Close() {
	delete s_Instance;
	glfwTerminate();
	exit(0);
}

}