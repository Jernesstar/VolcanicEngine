#include "Project.h"

#include <cstdlib>

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
}

void Project::Save() {
	// system("mingw32-make.exe -f Makefile");
}

void Project::Run() {
	// system("build/Project/bin/Project.exe");
}

}