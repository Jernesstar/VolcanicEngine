#include "Runtime.h"

#include <VolcaniCore/Event/Events.h>

#include <VolcaniCore/Graphics/Renderer.h>
#include <VolcaniCore/Graphics/ShaderLibrary.h>

#include <Magma/UI/UIRenderer.h>

#include "SceneLoader.h"
#include "UILoader.h"

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

	ShaderLibrary::Add(
		"Framebuffer", ShaderPipeline::Create("Asset/Shader", "Framebuffer"));
	ShaderLibrary::Add(
		"Lighting", ShaderPipeline::Create("Asset/Shader", "Lighting"));
	ShaderLibrary::Add(
		"Bloom", ShaderPipeline::Create("Asset/Shader", "Bloom"));

	UIRenderer::Init();

	Project project;
	project.Load("./.volc.proj");

	m_App = CreateRef<App>(project);
	m_AssetManager.Load();
	m_App->SetAssetManager(m_AssetManager);

	m_App->ScreenLoad =
		[&](Ref<ScriptModule> script)
		{
			auto scriptPath = fs::path("Class") / script->Name;
			script->Load(scriptPath.string() + ".class");
		};
	m_App->SceneLoad =
	[&](Scene& scene)
	{
		auto scenePath = fs::path("Scene") / scene.Name;
		SceneLoader::Load(scene, scenePath.string() + ".bin");
	};
	m_App->UILoad =
		[&](UIPage& page)
		{
			auto uiPath = fs::path("UI") / page.Name;
			UILoader::Load(page, uiPath.string() + ".bin");
		};

	m_App->ChangeScreen = true;
	m_App->Running = true;
	m_App->OnLoad();
}

Runtime::~Runtime() {
	m_App->OnClose();

	UIRenderer::Close();
}

void Runtime::OnUpdate(TimeStep ts) {
	m_App->OnUpdate(ts);
}

}