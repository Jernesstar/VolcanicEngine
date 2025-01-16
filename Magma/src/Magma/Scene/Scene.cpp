#include "Scene.h"

#include "ECS/Systems.h"

#include "SceneRenderer.h"

using namespace Magma::ECS;
using namespace Magma::Physics;

namespace Magma {

Scene::Scene(const std::string& name)
	: Name(name)
{
	RegisterSystems();
}

Scene::~Scene() {

}

void Scene::OnUpdate(TimeStep ts) {
	EntityWorld.OnUpdate(ts);
}

void Scene::OnRender(SceneRenderer& renderer) {
	// renderer.Submit();
}

void Scene::RegisterSystems() {
	EntityWorld.Add<ScriptSystem>({ Phase::PreUpdate });
	EntityWorld.Add<PhysicsSystem>(
		{ Phase::PreUpdate, Phase::OnUpdate, Phase::PostUpdate });
	EntityWorld.Add<TransformSystem>({ Phase::OnUpdate });
}

}