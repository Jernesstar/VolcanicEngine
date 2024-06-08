#include "Application.h"

#include <glad/glad.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "Assert.h"
#include "Events/Events.h"
#include "Renderer/Shader.h"
#include "Renderer/Renderer.h"

namespace VolcaniCore {

void ErrorCallback(int error, const char* description) {
	VOLCANICORE_ASSERT(false, description);
}

Application::Application() {
	s_Instance = this;
}

void Application::Init() {
	glfwSetErrorCallback(ErrorCallback);
	VOLCANICORE_ASSERT(glfwInit(), "Failed to initialize GLFW");

	s_Window = CreateRef<Window>(800, 600);

	s_BackendRenderer = RendererAPI::CreateRenderer(RenderAPI::OpenGL);
	ShaderPipeline::Init();
	s_BackendRenderer->Init();

	Renderer::Init();
	EventSystem::Init();
}

void Application::Close() {
	delete s_Instance;
	// glfwTerminate();
	exit(0);
}

void Application::Run() {
	while(s_Window->IsOpen()) {
		TimePoint time = Time::GetTime();
		TimeStep ts = time - s_LastFrame;
		s_LastFrame = time;
		s_TimeStep = ts;

		s_Instance->OnUpdate(ts);

		ApplicationUpdatedEvent event(ts);
		EventSystem::Dispatch(event);
		EventSystem::PollEvents();

		s_Window->Update();
	}
}

}
