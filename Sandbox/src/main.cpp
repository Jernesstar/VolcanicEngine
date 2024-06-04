#include <iostream>

#include <VolcaniCore/Core/Application.h>
#include <VolcaniCore/Core/CommandLineArgs.h>

#include <Magma/Editor.h>

#include "Demos/Cube.h"
#include "Demos/ECS.h"
#include "Demos/Lighting.h"
#include "Demos/Model.h"
#include "Demos/Template.h"

#include "TheMazeIsLava/Game.h"

VolcaniCore::Application* CreateApplication(const VolcaniCore::CommandLineArgs& args) {
	std::string project = args[1];

	if(project == "Template")
		return new TemplateDemo();
	if(project == "Cube")
		return new CubeDemo();
	if(project == "ECS")
		return new ECSDemo();
	if(project == "Lighting")
		return new LightingDemo();
	if(project == "Model")
		return new ModelDemo();
	if(project == "Editor")
		return new Magma::Editor();

	return new TemplateDemo();
}
