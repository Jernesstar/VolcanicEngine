#include <cstdlib>

#include "Application.h"

#include "Assert.h"

#include "Event/Events.h"

#include "Renderer/Renderer.h"
#include "Renderer/RendererAPI.h"
#include "Renderer/ShaderLibrary.h"

namespace fs = std::filesystem;

namespace VolcaniCore {

Application::Application(uint32_t width, uint32_t height,
						 const std::string& title, const std::string& iconPath)
{
	s_Window->Resize(width, height);
	s_Window->SetIcon(iconPath);
	s_Window->SetTitle(title);
	s_Instance = this;
}

void Application::Init() {
	VOLCANICORE_ASSERT(glfwInit(), "Failed to initialize GLFW");

	s_Window = CreateRef<Window>(800, 600);
	Events::Init();

	RendererAPI::Create(RendererAPI::Backend::OpenGL);
	ShaderLibrary::Init();
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

		float fps = (1.0f / (float)ts) * 1000.0f;
		Renderer::GetFrame().Info.FPS = fps;

		Events::PollEvents();

		Renderer::BeginFrame();
		{
			s_Instance->OnUpdate(ts);
		}
		Renderer::EndFrame();

		s_Window->Update();
	}
}

static std::string s_OldPath;

std::string Application::GetCurrentDir() {
	return s_Path;
}

void Application::PushDir() {
	PushDir(s_LibraryPath);
}

void Application::PushDir(const std::string& path) {
	if(path == "")
		return;

	s_OldPath = s_Path;
	s_Path = path;
	fs::current_path(s_Path);
}

void Application::PopDir() {
	s_Path = s_OldPath;
	fs::current_path(s_Path);
}

void Application::SetCurrentDir(const char* path) {
	s_LibraryPath = getenv("VOLC_PATH");

	fs::path p(path);
	if(p.stem() == "Sandbox")
		s_Path = s_LibraryPath;
	else
		s_Path = fs::current_path().string();
	s_OldPath = s_Path;
}

}
