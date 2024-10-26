#include "Project.h"

namespace Magma {

Project::Project(const std::filesystem::path& path) {
	Load(path);
}

Project::~Project() {

}

void Project::Load(const std::filesystem::path& path) {
	m_RootPath = path.parent_path().string();
	m_Name = path.stem().string();
}

void Project::Save(const std::filesystem::path& path) {
	
}

}