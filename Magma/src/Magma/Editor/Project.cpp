#include "Project.h"

#include <cstdlib>

#include <VolcaniCore/Core/Log.h>

namespace fs = std::filesystem;

namespace Magma {

Project::Project(const fs::path& volcFilePath) {
	Load(volcFilePath);
}

Project::~Project() {

}

void Project::Load(const fs::path& volcFilePath) {
	if(volcFilePath == "")
		return;

	auto fullPath = fs::canonical(volcFilePath).parent_path();
	m_Name = fullPath.stem().string();
	m_RootPath = fullPath.string();
	m_SrcPath   = (fullPath / m_Name / "src").string();
	m_AssetPath = (fullPath / m_Name / "assets").string();

	// Reload();
}

void Project::Reload() {
	// TODO(Implement): Dependencies
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
	// VOLCANICORE_LOG_INFO(command.c_str());
	system(command.c_str());

	command = "powershell Start-Process mingw32-make.exe -NoNewWindow";
	command += " -WorkingDir Magma\\projects\\build";
	command += " -ArgumentList '-f Makefile';";
	system(command.c_str());
}

void Project::Run() {
	std::string command;

	command = "powershell $ExePath = Resolve-Path";
	command += " Magma\\projects\\Project\\build\\bin\\Project.exe;";
	command += "Start-Process $ExePath -WorkingDir \'" + m_RootPath + "\';";
	system(command.c_str());
}

}