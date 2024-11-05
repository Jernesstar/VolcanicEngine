#include "Project.h"

#include <cstdlib>

#include <VolcaniCore/Core/Log.h>

namespace fs = std::filesystem;

namespace Magma {

Project::Project(const fs::path& path) {
	Load(path);
}

Project::~Project() {

}

void Project::Load(const fs::path& path) {
	auto fullPath = fs::canonical(path).parent_path();
	m_Name = fullPath.stem().string();
	m_RootPath = fullPath.string();
	m_SrcPath   = (fullPath / m_Name / "src").string();
	m_AssetPath = (fullPath / m_Name / "assets").string();

	// Reload();
}

void Project::Reload() {
	// TODO(Implement): Dependencies
	std::string command;
#ifdef VOLCANICENGINE_LINUX
	command = "vendor\\premake\\bin\\Linux\\premake5.exe";
#elif VOLCANICENGINE_WINDOWS
	command = "vendor\\premake\\bin\\Windows\\premake5.exe";
#endif
	command += " gmake2 --file=Magma\\projects\\premake5.lua";
	command += " --project=\"" + m_RootPath + "\"";
	command += " --src=\""     + m_SrcPath  + "\"";
	system(command.c_str());

	command = "powershell cd Magma\\projects\\build;";
	command += "mingw32-make.exe -f Makefile;";
	command += "if($Error.count -eq 0) {";
	command +=		"\'Project built successfully\'";
	command += "}";
	command += "else {";
	command +=		"\'Project encountered build errors\'";
	command += "}";
	system(command.c_str());
}

void Project::Run() {
	std::string command;

	command = "powershell";
	command += " $ExePath = Resolve-Path Magma\\projects\\Project\\build\\bin\\Project.exe;";
	command += " cd \'" + m_RootPath + "\';";
	command += " Start-Process $ExePath -Wait;";
	system(command.c_str());
}

}