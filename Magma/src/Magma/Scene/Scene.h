#pragma once

#include <vector>

#include <Core/Time.h>
#include <Renderer/Camera.h>
#include "Renderer/Mesh.h>

#include "EntitySystem.h"

namespace Magma {

class Scene {
public:
	const std::string Name;
	VolcaniCore::Ref<VolcaniCore::Camera> Camera;

public:
	Scene()
		: Name("Untitled") { }
	Scene(const std::string& name)
		: Name(name) { }
	~Scene() = default;

	EntitySystem& GetEntitySystem() { return m_EntitySystem; }

	void OnUpdate(TimeStep ts);
	void OnRender();

	void AddLight(const Light& light) {
		m_Lights.push_back(light);
	}

private:
	EntitySystem m_EntitySystem;

	std::vector<Light> m_Lights;
	std::vector<Ref<Mesh>> m_Meshes;
};

}