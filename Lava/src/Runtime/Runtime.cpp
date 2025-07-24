#include "Runtime.h"

#include <VolcaniCore/Event/Events.h>

#include <VolcaniCore/Graphics/Renderer.h>
#include <VolcaniCore/Graphics/Shader.h>
#include <VolcaniCore/Graphics/ShaderLibrary.h>

#include <Magma/Core/BinaryReader.h>
#include <Magma/Script/ScriptEngine.h>
#include <Magma/Audio/AudioEngine.h>
#include <Magma/Physics/Physics.h>
#include <Magma/UI/UIRenderer.h>

#include <Lava/Core/ScriptGlue.h>

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

	UIRenderer::Init();
	ImGuiIO& io = ImGui::GetIO();
	io.IniFilename = nullptr;

	Physics::Init();
	AudioEngine::Init();
	ScriptEngine::Init();
	ScriptGlue::RegisterInterface();

	Project project;
	project.Load("./.volc.proj");

	Application::GetWindow()->SetTitle(project.Name);
	m_AssetManager.Load();
	// Application::GetWindow()->SetIcon();

	for(std::string name : { "Framebuffer", "Lighting", "Mesh", "Light",
		"Bloom", "Downsample", "Upsample",
		"Particle", "ParticleEmitter", "ParticleUpdate" })
	{
		List<Shader> shaders;
		for(std::string type : { "vert", "frag", "comp" }) {
			auto path = "Asset/Shader/" + name + ".bin." + type;
			if(!FileUtils::FileExists(path))
				continue;

			ShaderType shaderType;
			if(type == "vert")
				shaderType = ShaderType::Vertex;
			else if(type == "frag")
				shaderType = ShaderType::Fragment;
			else if(type == "comp")
				shaderType = ShaderType::Compute;

			BinaryReader shaderFile(path);
			uint64_t count;
			shaderFile.Read(count);
			Buffer<void> buf(sizeof(uint32_t), count);
			shaderFile.ReadData(buf.Get(), buf.GetMaxSize());
			shaders.AddMove({ shaderType, std::move(buf) });
		}

		if(name == "Downsample") {
			name = "Bloom-Downsample";
			std::string framebuffer = "Asset/Shader/Framebuffer.bin.vert";
			BinaryReader shaderFile(framebuffer);
			uint64_t count;
			shaderFile.Read(count);
			Buffer<void> buf(sizeof(uint32_t), count);
			shaderFile.ReadData(buf.Get(), buf.GetMaxSize());
			shaders.AddMove({ ShaderType::Vertex, std::move(buf) });
		}
		else if(name == "Upsample") {
			name = "Bloom-Upsample";
			std::string framebuffer = "Asset/Shader/Framebuffer.bin.vert";
			BinaryReader shaderFile(framebuffer);
			uint64_t count;
			shaderFile.Read(count);
			Buffer<void> buf(sizeof(uint32_t), count);
			shaderFile.ReadData(buf.Get(), buf.GetMaxSize());
			shaders.AddMove({ ShaderType::Vertex, std::move(buf) });
		}
		else if(name == "Bloom") {
			std::string framebuffer = "Asset/Shader/Framebuffer.bin.vert";
			BinaryReader shaderFile(framebuffer);
			uint64_t count;
			shaderFile.Read(count);
			Buffer<void> buf(sizeof(uint32_t), count);
			shaderFile.ReadData(buf.Get(), buf.GetMaxSize());
			shaders.AddMove({ ShaderType::Vertex, std::move(buf) });
		}
		else if(name == "Particle")
			name = "Particle-DefaultDraw";
		else if(name == "ParticleEmitter")
			name = "Particle-Emit";
		else if(name == "ParticleUpdate")
			name = "Particle-Update";

		ShaderLibrary::Add(name, ShaderPipeline::Create(shaders));
	}

	m_App = CreateRef<App>();
	m_App->SetProject(project);
	m_App->AppLoad =
		[app = project.App](Ref<ScriptModule>& script)
		{
			auto handle =
				ScriptEngine::Get()->GetModule(app.c_str(), asGM_ALWAYS_CREATE);
			BinaryReader reader("./.volc.class");
			ByteCodeReader stream(&reader);
			handle->LoadByteCode(&stream);
			script = CreateRef<ScriptModule>(handle);
		};
	m_App->ScreenLoad =
		[](Ref<ScriptModule>& script, const std::string& name)
		{
			auto handle =
				ScriptEngine::Get()->GetModule(name.c_str(), asGM_ALWAYS_CREATE);
			BinaryReader reader((fs::path("Class") / name).string() + ".class");
			ByteCodeReader stream(&reader);
			handle->LoadByteCode(&stream);
			script = CreateRef<ScriptModule>(handle);
		};
	m_App->SceneLoad =
		[](Scene& scene)
		{
			auto scenePath = fs::path("Scene") / scene.Name;
			SceneLoader::Load(scene, scenePath.string() + ".bin");
		};
	m_App->UILoad =
		[](UIPage& page)
		{
			auto uiPath = fs::path("UI") / page.Name;
			UILoader::Load(page, uiPath.string() + ".bin");
		};

	m_App->ChangeScreen = true;
	m_App->RenderScene = true;
	m_App->RenderUI = true;
	m_App->Running = true;

	m_App->OnLoad();
	m_App->ScreenSet(project.StartScreen);
}

Runtime::~Runtime() {
	m_App->OnClose();
	m_App.reset();

	m_AssetManager.Clear();

	ScriptEngine::Shutdown();
	AudioEngine::Shutdown();
	Physics::Close();
	UIRenderer::Close();
}

void Runtime::OnUpdate(TimeStep ts) {
	Renderer::Clear();
	UIRenderer::BeginFrame();

	m_App->OnUpdate(ts);

	UIRenderer::EndFrame();
}

}