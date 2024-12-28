#include "Runtime.h"

#include <VolcaniCore/Event/Events.h>

#include <VolcaniCore/Graphics/RendererAPI.h>

#include <Lava/ProjectLoader.h>
#include <Lava/UIBrowser.h>

namespace Lava {

Runtime::Runtime(const std::string& volcPath)
	: Application(1920, 1080)
{
	Events::RegisterListener<KeyPressedEvent>(
		[](const KeyPressedEvent& event)
		{
			if(event.Key == Key::Escape)
				Application::Close();
		});

	Project project;
	ProjectLoader::Load(project, volcPath);
	ProjectLoader::Compile(volcPath);

	m_AppDLL = ProjectLoader::GetDLL();

	Application::GetWindow()->SetTitle(project.Name);
	Application::PushDir(project.Path);

	m_AppDLL->GetFunction<void>("LoadApp")();

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
	RendererAPI::Get()->NewDrawCommand()->Clear = true;
	RendererAPI::Get()->EndFrame();

	App* app = Get();
	app->OnUpdate(ts);
	app->App::OnUpdate(ts);
}

App* Runtime::Get() {
	auto get = m_AppDLL->GetFunction<App*>("GetApp");
	return get();
}

}