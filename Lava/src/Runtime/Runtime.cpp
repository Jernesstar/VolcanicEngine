#include "Runtime.h"

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

	if(!args["--project"]) {
		VOLCANICORE_LOG_ERROR("No project specified");
		Application::Close();
	}

	std::string volcPath = args["--project"];
	Project project;
	ProjectLoader::Load(project, volcPath);

	if(!args.Has("-c"))
		ProjectLoader::Compile(project);

	m_AppDLL = ProjectLoader::GetDLL();
	if(!m_AppDLL)
		VOLCANICORE_LOG_ERROR(
			"-c Flag used, though no DLLs could be found. \
			Make sure to have previously called ProjectLoader::Compile");

	m_AppDLL->GetFunction<void>("LoadApp")();

	Application::GetWindow()->SetTitle(project.Name);
	Application::PushDir(project.Path);

	App* app = Get();
	app->App::OnLoad();
	app->OnLoad();
}

Runtime::~Runtime() {
	App* app = Get();
	app->OnClose();
	app->App::OnClose();
}

void Runtime::OnUpdate(TimeStep ts) {
	App* app = Get();
	app->App::OnUpdate(ts);
	app->OnUpdate(ts);
}

App* Runtime::Get() {
	auto get = m_AppDLL->GetFunction<App*>("GetApp");
	return get();
}

}