#pragma once

#include <VolcaniCore/Core/Defines.h>
#include <VolcaniCore/Core/List.h>

#include "Screen.h"

using namespace VolcaniCore;

namespace Magma {

class Project {
public:
	std::string Path;
	std::string ExportPath;
	std::string Name;
	std::string App;
	std::string StartScreen;
	List<Screen> Screens;

public:
	Project();
	~Project();

	void AddScreen(const std::string& name,
				   const std::string& scene, const std::string& ui);
};

}