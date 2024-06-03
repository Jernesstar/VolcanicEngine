#include <iostream>

#include <VolcaniCore/Core/Application.h>
#include <VolcaniCore/Core/CommandLineArgs.h>

#include <Magma/Editor.h>

#include "Demos/ECS.h"
#include "Demos/Lighting.h"
#include "Demos/FrameBuffer.h"
#include "Demos/Model.h"
#include "Demos/Template.h"

#include "TheMazeIsLava/Game.h"

VolcaniCore::Application* CreateApplication(const VolcaniCore::CommandLineArgs& args) {
	std::string project = args[1];

	if(project == "Template")
		return new TemplateDemo();
	if(project == "Lighting")
		return new LightingDemo();
	if(project == "FrameBuffer")
		return new FrameBuferDemo();
	if(project == "Model")
		return new ModelDemo();
	if(project == "ECS")
		return new ECSDemo();
	if(project == "Editor")
		return new Magma::Editor();

	return new TemplateDemo();
}
