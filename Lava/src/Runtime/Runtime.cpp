#include "Runtime.h"

#ifdef VOLCANICENGINE_WINDOWS
	#include <windows.h>
#elif VOLCANICENGINE_LINUX
	#include <unistd.h>
#endif

#include <filesystem>

#include <VolcaniCore/Event/Events.h>
#include <VolcaniCore/Graphics/Renderer.h>

#include <Magma/UI/UIRenderer.h>

#include <Lava/ProjectLoader.h>

namespace fs = std::filesystem;

namespace Lava {

Runtime::Runtime(const CommandLineArgs& args)
	: Application(1920, 1080)
{
	Events::RegisterListener<KeyPressedEvent>(
		[](const KeyPressedEvent& event)
		{
			if(event.Key == Key::Escape)
				Application::Close();
		});

	std::string volcPath;
	if(args["--project"]) {
		volcPath = args["--project"].Args[0];
	}
	else {
#ifdef VOLCANICENGINE_WINDOWS
		char buf[2*MAX_PATH];
		size_t len = GetModuleFileName(0, buf, 2*MAX_PATH);
#elif VOLCANICENGINE_LINUX
		char buf[2*PATH_MAX];
		size_t len = readlink("/proc/self/exe", buf, 2*PATH_MAX);
#endif
		auto rootPath = fs::path(std::string(buf, len)).parent_path();
		volcPath = (rootPath / ".volc.proj").string();
	}

	Project project;
	ProjectLoader::Load(project, volcPath);

	m_App = CreateRef<App>(project);
	m_App->OnLoad();
}

Runtime::~Runtime() {
	m_App->OnClose();
}

void Runtime::OnUpdate(TimeStep ts) {
	m_App->OnUpdate(ts);
}

}