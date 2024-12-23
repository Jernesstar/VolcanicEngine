#include <VolcaniCore/Core/CommandLineArgs.h>

#include "Runtime.h"

Application* CreateApplication(const CommandLineArgs& args) {
	return new Lava::Runtime(args[0]);
}