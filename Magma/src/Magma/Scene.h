#pragma once

// #include <vector>

#include <VolcaniCore/Renderer/Camera.h>
// #include "VolcaniCore/Renderer/Mesh.h>


#include "EntitySystem.h"

namespace Magma {

// class Light;

class Scene {
public:
	const std::string Name;
	VolcaniCore::Ref<VolcaniCore::Camera> Camera;

public:
	Scene() : Name("Untitled") { }
	Scene(const std::string_view name) : Name(name) { }
	~Scene() = default;

	EntitySystem& GetEntitySystem() { return m_EntitySystem; }

private:
	EntitySystem m_EntitySystem;

	// std::vector<Light> m_Lights;
	// std::vector<Mesh> m_Meshes;
};

}