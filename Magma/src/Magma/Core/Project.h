#pragma once

#include <VolcaniCore/Core/Defines.h>

#include "YAMLSerializer.h"

using namespace VolcaniCore;

namespace Magma {

class Project {
public:
	std::string Path;
	std::string Name;
	std::string App;

public:
	Project();
	~Project();

	void AddClass(const std::string& name);
	void AddComponent(const std::string& name);
};

}