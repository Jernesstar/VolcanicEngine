#pragma once

#include <VolcaniCore/Core/Defines.h>
#include <VolcaniCore/Core/List.h>

#include "Screen.h"

using namespace VolcaniCore;

namespace Magma {

class Project {
public:
	std::string Path;
	std::string Name;
	std::string App;
	std::string StartScreen;
	List<Screen> Screens;

public:
	Project();
	~Project();

	void AddClass(const std::string& name);
	void AddComponent(const std::string& name);
};

}