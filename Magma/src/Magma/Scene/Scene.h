#pragma once

#include <VolcaniCore/Core/Time.h>
#include <VolcaniCore/Renderer/Renderer.h>
#include <VolcaniCore/Renderer/RenderPass.h>
#include <VolcaniCore/Renderer/Camera.h>
#include <VolcaniCore/Renderer/CameraController.h>

#include "ECS/World.h"
#include "Physics/World.h"

#include "Light.h"

using namespace VolcaniCore;

namespace Magma {

class Scene {
public:
	const std::string Name;

public:
	Scene(const std::string& name = "Untitled Scene");
	~Scene();

	void OnUpdate(TimeStep ts);
	void OnRender();

	void SetCamera(Ref<Camera> camera) { m_Camera = camera; }
	Ref<Camera> GetCamera() const { return m_Camera; }

	ECS::World& GetEntityWorld() { return m_EntityWorld; }
	Physics::World& GetPhysicsWorld() { return m_PhysicsWorld; }

private:
	ECS::World m_EntityWorld;
	Physics::World m_PhysicsWorld;

	// TODO(Maybe): Turn Camera and CameraController into Entitys
	Ref<Camera> m_Camera;
	CameraController m_Controller;
	List<Ref<Light>> m_Lights;

	Ref<RenderPass> m_DrawPass;
	flecs::system m_RenderSystem;

private:
	void RegisterSystems();
	void RegisterObservers();

	friend class Editor;
};

}