#include <iostream>

#include <Core/Application.h>
#include <Core/CommandLineArgs.h>

#include "Demos/Model.h"
#include "Demos/Lighting.h"
#include "Demos/Empty.h"

#include "TheMazeIsLava/Game.h"

VolcaniCore::Application* CreateApplication(const VolcaniCore::CommandLineArgs& args) {
	std::string project = args[1];

	if(project == "Empty")
		return new EmptyDemo();
	if(project == "Lighting")
		return new LightingDemo();
	if(project == "Model")
		return new ModelDemo();

	return new EmptyDemo();
}
