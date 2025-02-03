#include <VolcaniCore/Core/Application.h>
#include <VolcaniCore/Core/Log.h>
#include <VolcaniCore/Core/List.h>
#include <VolcaniCore/Core/Defines.h>
#include <VolcaniCore/Core/CommandLineArgs.h>

#include <VolcaniCore/Event/Events.h>

#include <VolcaniCore/Graphics/RendererAPI.h>
#include <VolcaniCore/Graphics/Renderer.h>
#include <VolcaniCore/Graphics/Renderer2D.h>
#include <VolcaniCore/Graphics/Renderer3D.h>
#include <VolcaniCore/Graphics/Camera.h>
#include <VolcaniCore/Graphics/StereographicCamera.h>
#include <VolcaniCore/Graphics/OrthographicCamera.h>
#include <VolcaniCore/Graphics/CameraController.h>

#include <VolcaniCore/Graphics/Shader.h>
#include <VolcaniCore/Graphics/Mesh.h>
#include <VolcaniCore/Graphics/Model.h>
#include <VolcaniCore/Graphics/Texture.h>

#include <imgui/imgui.h>

#include <Magma/UI/UI.h>

#include <Magma/ECS/World.h>
#include <Magma/ECS/Entity.h>
#include <Magma/ECS/EntityBuilder.h>

#include <Magma/Physics/Physics.h>
#include <Magma/Physics/RigidBody.h>
#include <Magma/Physics/Shape.h>
#include <Magma/Physics/World.h>

#include <Magma/Scene/Scene.h>
#include <Magma/Scene/Component.h>

#include <Lava/ProjectLoader.h>
#include <Lava/UILoader.h>
#include <Lava/SceneLoader.h>

using namespace VolcaniCore;
using namespace Magma;
using namespace Magma::ECS;
using namespace Magma::Physics;
using namespace Lava;

#include "Demos/Cube.h"
#include "Demos/Lighting.h"
#include "Demos/Shadows.h"
#include "Demos/Bloom.h"
#include "Demos/Deferred.h"
#include "Demos/UI.h"
#include "Demos/Text.h"
#include "Demos/ECS.h"
#include "Demos/Raycast.h"
#include "Demos/Collision.h"
#include "Demos/Script.h"
#include "Demos/Raytracing.h"
#include "Demos/Template.h"

Application* CreateApplication(const CommandLineArgs& args) {
	std::string project = args["--project"];

	// Isometric Camera, smooth scroll pixels, instancing
	if(project == "Cube") return new Demo::Cube();
	// PointLights, Spotlights, uniform buffers
	else if(project == "Lighting") return new Demo::Lighting();
	// Shadows
	else if(project == "Shadows") return new Demo::Shadows();
	// Bloom
	else if(project == "Bloom") return new Demo::Bloom();
	// Deferred
	else if(project == "Deferred") return new Demo::Deferred();
	// Text rendering
	else if(project == "Text") return new Demo::Text();
	// UI
	else if(project == "UI") return new Demo::UI();
	// Entity component system
	else if(project == "ECS") return new Demo::ECS();
	// Raycasting, object outlining
	else if(project == "Raycast") return new Demo::Raycast();
	// Collision detection
	else if(project == "Collision") return new Demo::Collision();
	// Scripts
	else if(project == "Script") return new Demo::Script();
	// PBR
	else if(project == "Raytracing") return new Demo::Raytracing();

	return new Demo::Template();
}
