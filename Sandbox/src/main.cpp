#include <VolcaniCore/Core/Application.h>
#include <VolcaniCore/Core/Log.h>
#include <VolcaniCore/Core/Defines.h>
#include <VolcaniCore/Core/CommandLineArgs.h>

#include <VolcaniCore/Event/Events.h>

#include <VolcaniCore/Graphics/Renderer.h>
#include <VolcaniCore/Graphics/RendererAPI.h>
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

#include <Magma/Scene/Scene.h>
#include <Magma/Scene/SceneSerializer.h>

#include <Magma/Editor/Editor.h>

#include <Magma/ECS/World.h>
#include <Magma/ECS/Entity.h>
#include <Magma/ECS/EntityBuilder.h>

#include <Magma/Physics/Physics.h>
#include <Magma/Physics/RigidBody.h>
#include <Magma/Physics/Shape.h>
#include <Magma/Physics/World.h>

using namespace VolcaniCore;
using namespace Magma;
using namespace Magma::ECS;
using namespace Magma::Physics;

#include "Demos/Cube.h"
#include "Demos/Lighting.h"
#include "Demos/Shadows.h"
#include "Demos/Bloom.h"
#include "Demos/Text.h"
#include "Demos/UI.h"
#include "Demos/ECS.h"
#include "Demos/Raycast.h"
#include "Demos/Collision.h"
#include "Demos/Raytracing.h"
#include "Demos/DLL.h"
#include "Demos/Template.h"

Application* CreateApplication(const CommandLineArgs& args) {
	std::string project = args[1];

	// Isometric Camera, smooth scroll pixels, instancing
	if(project == "Cube") return new Demo::Cube();
	// PointLights, Spotlights, uniform buffers
	if(project == "Lighting") return new Demo::Lighting();
	// Shadows
	if(project == "Shadows") return new Demo::Shadows();
	// Bloom
	if(project == "Bloom") return new Demo::Bloom();
	// Text rendering
	if(project == "Text") return new Demo::Text();
	// UI
	if(project == "UI") return new Demo::UILayer();
	// Entity component system
	if(project == "ECS") return new Demo::ECS();
	// Raycasting, object outlining
	if(project == "Raycast") return new Demo::Raycast();
	// Collision detection
	if(project == "Collision") return new Demo::Collision();
	// PBR
	if(project == "Raytracing") return new Demo::Raytracing();
	// DLL
	if(project == "DLL") return new Demo::DLL();
	// Magma Editor
	if(project == "Editor") return new Magma::Editor();

	return new Demo::Template();
}
