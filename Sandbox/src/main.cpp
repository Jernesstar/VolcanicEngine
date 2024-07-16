#include <iostream>

#include <VolcaniCore/Core/Application.h>
#include <VolcaniCore/Core/Log.h>
#include <VolcaniCore/Core/CommandLineArgs.h>

#include <VolcaniCore/Events/EventSystem.h>

#include <VolcaniCore/Renderer/Renderer.h>
#include <VolcaniCore/Renderer/RendererAPI.h>
#include <VolcaniCore/Renderer/Renderer2D.h>
#include <VolcaniCore/Renderer/Renderer3D.h>
#include <VolcaniCore/Renderer/Camera.h>
#include <VolcaniCore/Renderer/StereographicCamera.h>
#include <VolcaniCore/Renderer/OrthographicCamera.h>
#include <VolcaniCore/Renderer/CameraController.h>

#include <VolcaniCore/Object/Shader.h>
#include <VolcaniCore/Object/Mesh.h>
#include <VolcaniCore/Object/Model.h>
#include <VolcaniCore/Object/Texture.h>

#include <Magma/Editor/Editor.h>
#include <Magma/Physics/Physics.h>
#include <Magma/UI/UIElements.h>

#include "Demos/Template.h"
#include "Demos/Lighting.h"
#include "Demos/Shadows.h"
#include "Demos/ECS.h"
#include "Demos/Physics.h"

#include "Sandbox/TheMazeIsLava/src/Game.h"

using namespace VolcaniCore;

Application* CreateApplication(const CommandLineArgs& args) {
	std::string project = args[1];

	if(project == "Lighting")  return new Demo::Lighting();
	if(project == "Shadows")   return new Demo::Shadows();
	if(project == "ECS")	   return new Demo::ECS();
	if(project == "Physics")   return new Demo::Physics();

	if(project == "Editor") return new Magma::Editor();
	if(project == "Game")	return new TheMazeIsLava::Game();

	return new Demo::Template();
}
