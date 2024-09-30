#pragma once

#include <VolcaniCore/Core/Time.h>
#include <VolcaniCore/Renderer/Camera.h>
#include <VolcaniCore/Renderer/CameraController.h>

#include "ECS/World.h"
#include "Physics/World.h"

#include "Light.h"

using namespace VolcaniCore;

namespace Magma {

class SceneRenderer;

class Scene {
public:
	const std::string Name;

public:
	Scene(const std::string& name = "Untitled Scene");
	~Scene();

	void OnUpdate(TimeStep ts);
	void OnRender();

	void SetCamera(Ref<Camera> camera);
	void SetController(const CameraController& controller);
	Ref<Camera> GetCamera() const { return m_Camera; }
	List<Ref<Light>>& GetLights() { return m_Lights; }

	ECS::World& GetEntityWorld() { return m_EntityWorld; }
	Physics::World& GetPhysicsWorld() { return m_PhysicsWorld; }

private:
	ECS::World m_EntityWorld;
	Physics::World m_PhysicsWorld;

	// TODO(Maybe): Turn into Entitys
	Ref<Camera> m_Camera;
	CameraController m_Controller;
	List<Ref<Light>> m_Lights;

	Ref<SceneRenderer> m_Renderer;

private:
	void RegisterSystems();
	void RegisterObservers();

	friend class Editor;
};

}