#include "Application.h"

#include <glad/glad.h>

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

#include "Assert.h"
#include "Time.h"
#include "Renderer/Renderer.h"
#include "Events/Events.h"

namespace VolcaniCore {

Application::Application() {
	m_Window = CreateRef<Window>(800, 600);
	s_Instance = this;
}

void Application::Init() {
	VOLCANICORE_ASSERT(glfwInit());

	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	// Renderer::Init();
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

        s_Instance->m_Window->Update();
    }
}

void Application::Close() {
	glfwTerminate();
	exit(0);
}

}