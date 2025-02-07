#include "Scene.h"

#include "PhysicsSystem.h"
#include "ScriptSystem.h"
#include "TransformSystem.h"

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

	renderer.Begin();

	world.query_builder()
	.with<CameraComponent>()
	.build()
	.each(
		[&](flecs::entity id)
		{
			renderer.SubmitCamera(Entity{ id });
		});

	world.query_builder()
	.with<SkyboxComponent>()
	.build()
	.each(
		[&](flecs::entity id)
		{
			renderer.SubmitSkybox(Entity{ id });
		});

	world.query_builder()
	.with<PointLightComponent>().or_()
	.with<SpotlightComponent>().or_()
	.with<DirectionalLightComponent>()
	.build()
	.each(
		[&](flecs::entity id)
		{
			renderer.SubmitLight(Entity{ id });
		});

	world.query_builder()
	.with<MeshComponent>().and_()
	.with<TransformComponent>()
	.build()
	.each(
		[&](flecs::entity id)
		{
			renderer.SubmitMesh(Entity{ id });
		});

	renderer.Render();
}

void Scene::RegisterSystems() {
	EntityWorld.Add<ScriptSystem>({ Phase::PreUpdate });
	EntityWorld.Add<PhysicsSystem>(
		{ Phase::PreUpdate, Phase::OnUpdate, Phase::PostUpdate });
	EntityWorld.Add<TransformSystem>({ Phase::OnUpdate });
}

}