#include "Project.h"

#include <cstdlib>

#include <VolcaniCore/Core/Log.h>

namespace Magma {

Project::Project(const std::filesystem::path& path) {
	Load(path);
}

Project::~Project() {
	Save();
}

void Project::Load(const std::filesystem::path& path) {
	m_RootPath = path.parent_path().string();
	m_Name = path.stem().string();

	// Configure .lua file
	// Find dependencies
	// system("cd Magma/projects/Project;"
	// 	   "/../../../vendor/premake/bin/Windows/premake5.exe gmake2");
}

void Project::Save() {
	// system("mingw32-make.exe -f build/projects/Makefile");
}

void Project::Run() {
	// system("build/projects/Project/build/bin/Project.exe");
}

}