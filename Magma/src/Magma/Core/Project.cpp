#include "Project.h"

namespace Magma {

Project::Project() {

}

Project::~Project() {

}

void Project::AddScreen(const std::string& name,
						const std::string& scene, const std::string& ui)
{
	Screens.Emplace(name, scene, ui);
}

}