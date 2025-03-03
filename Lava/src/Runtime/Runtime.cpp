#include "Runtime.h"

#include <VolcaniCore/Event/Events.h>
#include <VolcaniCore/Graphics/Renderer.h>

#include <Lava/ScriptGlue.h>

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

	std::string rootPath;
	if(args["--project"]) {
		auto volcPath = args["--project"].Args[0];
		rootPath = fs::path(volcPath).parent_path().string();
	}
	else
		rootPath = fs::path(FindExecutablePath()).parent_path().string();

	Application::PushDir(rootPath);

	Project project;
	project.Load("./.volc.proj");
	m_AssetManager.Load();

	ScriptEngine::Init();
	ScriptGlue::Init();

	m_App = CreateRef<App>(project);
	m_App->ChangeScreen = true;
	m_App->Running = true;
	m_App->SetAssetManager(m_AssetManager);

	m_App->OnLoad();
}

Runtime::~Runtime() {
	m_App->OnClose();
}

void Runtime::OnUpdate(TimeStep ts) {
	m_App->OnUpdate(ts);
}

}