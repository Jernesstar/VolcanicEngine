#include "Application.h"

#include <glad/glad.h>
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "Assert.h"
#include "Events/Events.h"
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

	s_Window = CreateRef<Window>(1200, 900);

	s_BackendRenderer = RendererAPI::CreateRenderer(RenderAPI::OpenGL);
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

		ApplicationUpdatedEvent event(ts);
		EventSystem::PollEvents();
		EventSystem::Dispatch(event);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		s_Instance->OnUpdate(ts);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		s_Window->Update();
	}
}

}
