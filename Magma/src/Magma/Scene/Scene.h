#pragma once

#include <VolcaniCore/Core/Time.h>

#include "ECS/World.h"

#include "SceneRenderer.h"

using namespace VolcaniCore;

namespace Magma {

class Scene {
public:
	std::string Name;

	ECS::World EntityWorld;

public:
	Scene(const std::string& name = "Untitled Scene");
	~Scene() = default;

	void OnUpdate(TimeStep ts);
	void OnRender(SceneRenderer& renderer);
	void RegisterSystems();
	void UnregisterSystems();
};

}