#pragma once

#include <VolcaniCore/Core/Defines.h>

using namespace VolcaniCore;

namespace Magma {

class Project {
public:
	std::string Name;

public:
	Project();
	~Project();

	void SetGameClass(const std::string& name);
	void AddClass(const std::string& name);
	void AddComponent(const std::string& name);
};

}