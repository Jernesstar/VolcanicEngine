#pragma once

#include <VolcaniCore/Camera/Camera.h>

#include "EntitySystem.h"

namespace Magma {

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
};

}