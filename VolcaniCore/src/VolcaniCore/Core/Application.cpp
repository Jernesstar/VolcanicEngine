#include <cstdlib>

#include "Application.h"
#include "Assert.h"

#include "Event/Events.h"

#include "Graphics/Renderer.h"
#include "Graphics/RendererAPI.h"
#include "Graphics/ShaderLibrary.h"

namespace fs = std::filesystem;

namespace VolcaniCore {

Application::Application(uint32_t width, uint32_t height,
						 const std::string& title, const ImageData& icon)
{
	s_Instance = this;
	s_Window->Resize(width, height);
	s_Window->SetIcon(icon);
	s_Window->SetTitle(title);
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

	ShaderLibrary::Clear();

	Renderer::Close();
	RendererAPI::Shutdown();

	s_Window.reset();
	glfwTerminate();
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

		RendererAPI::Get()->StartFrame();
		Renderer::BeginFrame();
		{
			s_Instance->OnUpdate(ts);
		}
		Renderer::EndFrame();
		RendererAPI::Get()->EndFrame();

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

void Application::SetCurrentDir() {
	char* env = getenv("VOLC_PATH");
	VOLCANICORE_ASSERT(env);

	s_LibraryPath = env;
	s_Path = fs::current_path().string();
	s_OldPath = s_Path;
}

}
