#include <iostream>

#include <VolcaniCore/Core/Application.h>
#include <VolcaniCore/Core/CommandLineArgs.h>

#include <Magma/Editor/Editor.h>

#include "Demos/Template.h"
#include "Demos/Lighting.h"
#include "Demos/Model.h"
#include "Demos/Shadows.h"
#include "Demos/Selection.h"

#include "Sandbox/TheMazeIsLava/src/Game.h"

using namespace VolcaniCore;

Application* CreateApplication(const CommandLineArgs& args) {
	std::string project = args[1];

	if(project == "Lighting")  return new Demo::Lighting();
	if(project == "Model")	   return new Demo::Model();
	if(project == "Shadows")   return new Demo::Shadows();
	if(project == "Physics")   return new Demo::Physics();

	if(project == "Editor") return new Magma::Editor();
	if(project == "Game")	return new TheMazeIsLava::Game();

	return new Demo::Template();
}
