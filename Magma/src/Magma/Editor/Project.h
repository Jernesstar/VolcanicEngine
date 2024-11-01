#pragma once

#include <filesystem>

#include <VolcaniCore/Core/Defines.h>

namespace Magma {

class Project {
public:
	Project() = default;
	Project(const std::filesystem::path& path);
	~Project();

	void Load(const std::filesystem::path& path);
	void Save();
	void Run();

	// void AddDependency(const std::string& pathUrl);

private:
	std::string m_Name;
	std::string m_RootPath;
	std::string m_AssetPath;
};

}