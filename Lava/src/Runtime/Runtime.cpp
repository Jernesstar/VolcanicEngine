#include "Runtime.h"

#include <VolcaniCore/Event/Events.h>

#include <VolcaniCore/Graphics/RendererAPI.h>

#include <Lava/ProjectLoader.h>

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
	// ProjectLoader::Load(project, volcPath);
	ProjectLoader::Compile(volcPath);

	Application::GetWindow()->SetTitle(project.Name);

	m_GameDLL = CreateRef<DLL>("Lava/projects/Project/build/bin/Project.dll");

	Game* game = Get();
	game->Game::OnLoad();
	game->OnLoad();
}

Runtime::~Runtime() {
	Game* game = Get();
	game->OnClose();
	game->Game::OnClose();
}

void Runtime::OnUpdate(TimeStep ts) {
	RendererAPI::Get()->NewDrawCommand()->Clear = true;
	RendererAPI::Get()->EndFrame();

	Game* game = Get();
	game->OnUpdate(ts);
	game->Game::OnUpdate(ts);
}

Game* Runtime::Get() {
	auto get = m_GameDLL->GetFunction<Game*>("GetGameClass");
	return get();
}

}