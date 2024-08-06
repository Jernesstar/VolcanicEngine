#pragma once

#include <Core/Time.h>

#include <Renderer/Camera.h>

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

	ECS::World& GetEntityWorld() { return m_EntityWorld; }
	Physics::World& GetPhysiscsWorld() { return m_PhysicsWorld; }

private:
	void RegisterSystems();
	void RegisterObservers();

	Ref<Camera> m_Camera;

	ECS::World m_EntityWorld;
	Physics::World m_PhysicsWorld;

	friend class Editor;
};

}