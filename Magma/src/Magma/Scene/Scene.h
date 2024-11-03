#pragma once

#include <VolcaniCore/Core/Time.h>

#include "ECS/World.h"
#include "Physics/World.h"

#include "SceneRenderer.h"

using namespace VolcaniCore;

namespace Magma {

class Scene {
public:
	std::string Name;

	ECS::World EntityWorld;
	Physics::World PhysicsWorld;

public:
	Scene(const std::string& name = "Untitled Scene");
	~Scene();

	void OnUpdate(TimeStep ts);
	void OnRender();

	void Load(const std::string& path);
	void Save(const std::string& path);

	void SetRenderer(const SceneRenderer& renderer) {
		m_Renderer = renderer;
		m_Renderer.SetContext(this);
	}
	SceneRenderer& GetRenderer() { return m_Renderer; }

private:
	SceneRenderer m_Renderer;
	SceneSerializer m_Serializer;

private:
	void RegisterSystems();
	void RegisterObservers();
};

}