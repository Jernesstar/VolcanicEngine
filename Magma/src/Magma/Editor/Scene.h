#pragma once

#include <VolcaniCore/Core/Time.h>

#include <VolcaniCore/Renderer/Camera.h>
#include <VolcaniCore/Renderer/CameraController.h>

#include "ECS/World.h"

#include "Physics/World.h"

using namespace VolcaniCore;

namespace Magma {

class Scene {
public:
	const std::string Name;

public:
	Scene(const std::string& name = "Untitled Scene");
	~Scene() = default;

	void OnUpdate(TimeStep ts);
	void OnRender();

	void SetCamera(Ref<Camera> camera) { m_Camera = camera; }
	Ref<Camera> GetCamera() const { return m_Camera; }

	ECS::World& GetEntityWorld() { return m_EntityWorld; }
	Physics::World& GetPhysiscsWorld() { return m_PhysicsWorld; }

private:
	void RegisterSystems();
	void RegisterObservers();

	Ref<Camera> m_Camera;
	Ref<CameraController> m_Controller;

	flecs::system m_RenderSys;
	ECS::World m_EntityWorld;
	Physics::World m_PhysicsWorld;

	friend class Editor;
};

}