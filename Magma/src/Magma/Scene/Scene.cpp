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
	auto& world = EntityWorld.GetNative();

	// Query?
	world
	.system<const TransformComponent, const MeshComponent>("Objects")
	.each(
		[&](flecs::entity id, const TransformComponent&, const MeshComponent&)
		{
			renderer.Submit(Entity{ id });
		})
	.run();
	// world
	// .system<const TransformComponent>("PointLights")
	// .run(
	// 	[&](flecs::entity id, const PointLightComponent&)
	// 	{
	// 		renderer.Submit(Entity{ id });
	// 	});
}

void Scene::RegisterSystems() {
	EntityWorld.Add<ScriptSystem>({ Phase::PreUpdate });
	EntityWorld.Add<PhysicsSystem>(
		{ Phase::PreUpdate, Phase::OnUpdate, Phase::PostUpdate });
	EntityWorld.Add<TransformSystem>({ Phase::OnUpdate });
}

}