#include "Runtime.h"

#include <filesystem>

#include <VolcaniCore/Event/Events.h>
#include <VolcaniCore/Graphics/Renderer.h>

#include <Magma/UI/UIRenderer.h>

#include <Lava/ProjectLoader.h>

namespace fs = std::filesystem;

namespace Lava {

extern std::string FindExecutablePath();

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
		auto rootPath = fs::path(FindExecutablePath()).parent_path();
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