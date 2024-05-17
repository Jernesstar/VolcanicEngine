#include <iostream>

#include <VolcaniCore/Core/Application.h>
#include <VolcaniCore/Core/CommandLineArgs.h>

#include "Demos/Template.h"
#include "Demos/Model.h"
#include "Demos/Lighting.h"
#include "Demos/ECS.h"

#include "TheMazeIsLava/Game.h"

VolcaniCore::Application* CreateApplication(const VolcaniCore::CommandLineArgs& args) {
	std::string project = args[1];

	if(project == "Template")
		return new TemplateDemo();
	if(project == "Lighting")
		return new LightingDemo();
	if(project == "Model")
		return new ModelDemo();
	if(project == "ECS")
		return new ECSDemo();

	return new TemplateDemo();
}
