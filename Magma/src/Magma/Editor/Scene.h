#pragma once

#include <Core/Time.h>

#include <Renderer/Camera.h>

#include "EntitySystem.h"

using namespace VolcaniCore;

namespace Magma {

class Scene {
public:
	const std::string Name;
	Ref<VolcaniCore::Camera> Camera;

public:
	Scene(const std::string& name = "Untitled Scene");
	~Scene() = default;

	void OnUpdate(TimeStep ts); // Run physics
	void OnRender(); // Render

	EntitySystem& GetEntitySystem() { return m_EntitySystem; }
	// PhysicsSystem& GetPhysiscsSystem() { return m_PhysicsSystem; }

private:
	EntitySystem m_EntitySystem;
	// PhysicsSystem m_PhysicsSystem;
};

}