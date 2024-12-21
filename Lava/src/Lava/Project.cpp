#include "Project.h"

#include <cstdlib>
#include <filesystem>

#include <VolcaniCore/Core/Log.h>
#include <VolcaniCore/Core/FileUtils.h>

#include "Magma/UI/UIBrowser.h"

#include "AssetManager.h"

using namespace Magma::UI;

namespace Lava {

Ref<Project> Project::Create(const std::string& volcPath) {
	return CreateRef<Project>(volcPath);
}

Project::Project(const std::string& volcPath) {
	Load(volcPath);
}

Project::~Project() {

}

void Project::Load(const std::string& volcPath) {
	namespace fs = std::filesystem;

	if(volcPath == "")
		return;

	auto fullPath = fs::canonical(volcPath).parent_path();
	m_Name = fullPath.stem().string();
	m_RootPath = fullPath.string();
	m_SrcPath   = (fullPath / m_Name / "src").string();
	m_AssetPath = (fullPath / m_Name / "asset").string();

	auto uiPath = (fs::path(m_AssetPath) / "ui").string();
	auto modelPath = (fs::path(m_AssetPath) / "model").string();

	// for(auto path : FileUtils::GetFiles(modelPath)) {
	// 	AssetManager::GetOrCreate<Mesh>(path);
	// }
}

void Project::Reload() {
	std::string command;
	command = "powershell 'Creating Makefiles';";
	command += "$ProjectPath = \'" + m_SrcPath + "\';";
	command += "Start-Process ";

#ifdef VOLCANICENGINE_LINUX
	command += "vendor\\premake\\bin\\Linux\\premake5.exe";
#elif VOLCANICENGINE_WINDOWS
	command += "vendor\\premake\\bin\\Windows\\premake5.exe";
#endif
	command += " -Wait -NoNewWindow";
	command += " -Args 'gmake2 --file=Magma\\projects\\premake5.lua',";
	command += " (' --src=' + '\"' + $ProjectPath + '\"');";
	command += "'Finished creating Makefiles'";
	system(command.c_str());

	command = "powershell Start-Process mingw32-make.exe -NoNewWindow -Wait";
	command += " -WorkingDir Magma\\projects\\build";
	command += " -ArgumentList '-f Makefile';";
	system(command.c_str());

	UIBrowser::Reload();
}

void Project::Run() {
	std::string command;

	command = "powershell $ExePath = Resolve-Path";
	command += " Magma\\projects\\Project\\build\\bin\\Project.exe;";
	command += "Start-Process $ExePath -WorkingDir \'" + m_RootPath + "\';";
	system(command.c_str());
}

}