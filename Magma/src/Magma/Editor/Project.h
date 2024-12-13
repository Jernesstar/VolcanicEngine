#pragma once

#include <VolcaniCore/Core/Defines.h>

using namespace VolcaniCore;

namespace Magma {

class Project {
public:
	static Ref<Project> Create(const std::string& volcPath);

public:
	Project() = default;
	Project(const std::string& volcPath);
	~Project();

	std::string GetName() const { return m_Name; }

	void Load(const std::string& volcPath);
	void Reload();
	void Run();

	// void AddDependency(const std::string& pathUrl);

private:
	std::string m_Name;
	std::string m_RootPath;
	std::string m_SrcPath;
	std::string m_AssetPath;
};

}