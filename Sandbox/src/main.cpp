#include <iostream>

#include <VolcaniCore/Core/Application.h>
#include <VolcaniCore/Core/CommandLineArgs.h>

#include <Magma/Editor/Editor.h>

#include "Demos/Cube.h"
#include "Demos/ECS.h"
#include "Demos/Lighting.h"
#include "Demos/Model.h"
#include "Demos/Template.h"

#include "Sandbox/TheMazeIsLava/src/Game.h"

VolcaniCore::Application* CreateApplication(
									const VolcaniCore::CommandLineArgs& args)
{
	std::string project = args[1];

	if(project == "Template")
		return new Demo::Template();
	if(project == "Cube")
		return new Demo::Cube();
	if(project == "ECS")
		return new Demo::ECS();
	if(project == "Model")
		return new Demo::Model();
	if(project == "Lighting")
		return new Demo::Lighting();

	if(project == "Editor")
		return new Magma::Editor();
	if(project == "Game")
		return new TheMazeIsLava::Game();

	return new Demo::Template();
}
