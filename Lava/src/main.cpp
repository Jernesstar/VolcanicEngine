#include <VolcaniCore/Core/CommandLineArgs.h>

#include <Lava/Runtime.h>

Application* CreateApplication(const CommandLineArgs& args) {
	return new Lava::Runtime(args[0]);
}