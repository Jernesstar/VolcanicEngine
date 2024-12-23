#include "ProjectLoader.h"

#include <VolcaniCore/Core/Log.h>
#include <VolcaniCore/Core/FileUtils.h>

#include <Magma/Core/YAMLSerializer.h>

#include "UIBrowser.h"
#include "UILoader.h"
#include "SceneLoader.h"

namespace fs = std::filesystem;

using namespace Magma;

namespace Lava {

static void LoadSource(YAML::Node& node, const std::string& srcPath);
static void LoadAssets(YAML::Node& node, const std::string& assetPath);

void ProjectLoader::Load(Project& page, const std::string& volcPath) {
	YAML::Node file;
	try {
		file = YAML::LoadFile(volcPath);
	}
	catch(YAML::ParserException e) {
		return;
	}

	auto fullPath = fs::canonical(volcPath);
	auto parentPath = fullPath.parent_path();
	auto name = parentPath.stem().string();
	auto rootPath = parentPath.string();
	auto srcPath   = (parentPath / "src").string();
	auto assetPath = (parentPath / "asset").string();

	LoadSource(file, srcPath);
	LoadAssets(file, assetPath);
}

void ProjectLoader::Save(const Project& proj, const std::string& volcPath) {

}

void ProjectLoader::Compile(const std::string& volcPath) {
	YAML::Node file;
	try {
		file = YAML::LoadFile(volcPath);
	}
	catch(YAML::ParserException e) {
		return;
	}

	auto fullPath = fs::canonical(volcPath);
	auto parentPath = fullPath.parent_path();
	auto name = parentPath.stem().string();
	auto rootPath = parentPath.string();
	auto srcPath   = (parentPath / "src").string();
	auto assetPath = (parentPath / "asset").string();

	auto uiPath = (fs::path(assetPath) / "ui").string();
	auto modelPath = (fs::path(assetPath) / "model").string();

	UIBrowser::Load(uiPath);

// 	std::string command;

// #ifdef VOLCANICENGINE_LINUX
// 	command += "vendor/premake/bin/Linux/premake5.exe";
// #elif VOLCANICENGINE_WINDOWS
// 	command = "powershell 'Creating Makefiles';";
// 	command += "$SrcPath = \'" + srcPath + "\';";
// 	command += "Start-Process ";

// 	command += "vendor\\premake\\bin\\Windows\\premake5.exe";
// 	command += " -Wait -NoNewWindow";
// 	command += " -Args 'gmake2 --file=Runtime\\projects\\premake5.lua',";
// 	command += " (' --src=' + '\"' + $SrcPath + '\"');";
// 	command += "'Finished creating Makefiles'";
// 	system(command.c_str());

// 	command = "powershell Start-Process mingw32-make.exe -NoNewWindow -Wait";
// 	command += " -WorkingDir Runtime\\projects\\build";
// 	command += " -ArgumentList '-f Makefile';";
// 	system(command.c_str());
// #endif

	UIBrowser::Reload();
}

void LoadSource(YAML::Node& node, const std::string& srcPath) {

}

void LoadAssets(YAML::Node& node, const std::string& assetPath) {


	// for(auto path : FileUtils::GetFiles(modelPath)) {
	// 	AssetManager::GetOrCreate<Mesh>(path);
	// }
}

}