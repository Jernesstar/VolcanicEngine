#include "Runtime.h"

#include <cstdlib>
#include <filesystem>

#include <VolcaniCore/Core/Log.h>
#include <VolcaniCore/Event/Events.h>
#include <VolcaniCore/Graphics/RendererAPI.h>

#include <VolcaniCore/Core/Log.h>
#include <VolcaniCore/Core/FileUtils.h>

#include <Magma/Core/YAMLSerializer.h>
#include <Magma/UI/UI.h>

#include "UIBrowser.h"
// #include "AssetManager.h"
#include "Game.h"

namespace fs = std::filesystem;

using namespace Magma::UI;

namespace Lava {

Runtime::Runtime(const std::string& volcPath)
	: Application(1920, 1080), m_RootPath(volcPath)
{
	Events::RegisterListener<KeyPressedEvent>(
		[](const KeyPressedEvent& event)
		{
			if(event.Key == Key::Escape)
				Application::Close();
		});

	Load();
	Reload();

	auto get = m_GameDLL->GetFunction<Game*>("GetGameClass");
	Game* game = get();
	game->OnLoad();

	UIRenderer::Init();
}

Runtime::~Runtime() {
	auto get = m_GameDLL->GetFunction<Game*>("GetGameClass");
	Game* game = get();
	game->OnClose();

	UIRenderer::Close();
}

void Runtime::OnUpdate(TimeStep ts) {
	RendererAPI::Get()->NewDrawCommand()->Clear = true;
	RendererAPI::Get()->EndFrame();

	auto get = m_GameDLL->GetFunction<Game*>("GetGameClass");
	Game* game = get();
	game->OnUpdate(ts);

	UIRenderer::BeginFrame();
	UIBrowser::OnUpdate(ts);
	UIRenderer::EndFrame();
}

static void LoadSource(const std::string& volcPath);
static void LoadAssets(const std::string& assetPath);

void Runtime::Load() {
	if(m_RootPath == "")
		return;

	auto fullPath = fs::canonical(m_RootPath).parent_path();
	m_Name = fullPath.stem().string();
	m_RootPath = fullPath.string();
	m_SrcPath   = (fullPath / "src").string();
	m_AssetPath = (fullPath / "asset").string();

	LoadSource(m_RootPath);
	LoadAssets(m_AssetPath);
}

void Runtime::Reload() {
	std::string command;

#ifdef VOLCANICENGINE_LINUX
	command += "vendor/premake/bin/Linux/premake5.exe";
#elif VOLCANICENGINE_WINDOWS
	command = "powershell 'Creating Makefiles';";
	command += "$SrcPath = \'" + m_SrcPath + "\';";
	command += "Start-Process ";

	command += "vendor\\premake\\bin\\Windows\\premake5.exe";
	command += " -Wait -NoNewWindow";
	command += " -Args 'gmake2 --file=Runtime\\projects\\premake5.lua',";
	command += " (' --src=' + '\"' + $SrcPath + '\"');";
	command += "'Finished creating Makefiles'";
	system(command.c_str());

	command = "powershell Start-Process mingw32-make.exe -NoNewWindow -Wait";
	command += " -WorkingDir Runtime\\projects\\build";
	command += " -ArgumentList '-f Makefile';";
	system(command.c_str());
#endif

	m_GameDLL = CreateRef<DLL>("Lava/projects/Project/build/bin/Project.dll");

	UIBrowser::Reload();
}

void LoadSource(const std::string& volcPath) {
	YAML::Node file;
	try {
		file = YAML::LoadFile(volcPath);
	}
	catch(YAML::ParserException e) {
		return;
	}

	auto srcPath = (fs::path(volcPath).parent_path() / "src").string();
}

void LoadAssets(const std::string& assetPath) {
	auto uiPath = (fs::path(assetPath) / "ui").string();
	auto modelPath = (fs::path(assetPath) / "model").string();

	UIBrowser::Load(uiPath);

	// for(auto path : FileUtils::GetFiles(modelPath)) {
	// 	AssetManager::GetOrCreate<Mesh>(path);
	// }
}

}