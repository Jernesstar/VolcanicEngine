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
	void Save(const std::filesystem::path& path);

private:
	std::string m_RootPath;
	std::string m_Name;
};

}