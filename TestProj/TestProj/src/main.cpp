#include <VolcaniCore/Core/Application.h>
#include <VolcaniCore/Core/CommandLineArgs.h>

using namespace VolcaniCore;

Application* CreateApplication(const CommandLineArgs& args) {
	return new Application;
}