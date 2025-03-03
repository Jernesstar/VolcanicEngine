#include "Scene.h"

#include "PhysicsSystem.h"
#include "ScriptSystem.h"

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
	EntityWorld.Remove<PhysicsSystem>();
	EntityWorld.Remove<ScriptSystem>();
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
	EntityWorld.Add<PhysicsSystem>();
	EntityWorld.Add<ScriptSystem>();

	for(auto phase : { flecs::PreUpdate, flecs::OnUpdate, flecs::PostUpdate }) {
		Phase ourPhase;
		if(phase == flecs::PreUpdate)
			ourPhase = Phase::PreUpdate;
		if(phase == flecs::OnUpdate)
			ourPhase = Phase::OnUpdate;
		if(phase == flecs::PostUpdate)
			ourPhase = Phase::PostUpdate;

		EntityWorld.GetNative()
		.system<RigidBodyComponent>()
		.kind(phase)
		.run(
			[&](flecs::iter& it)
			{
				auto sys = EntityWorld.Get<PhysicsSystem>();
				if(!sys)
					return;

				if(ourPhase == Phase::OnUpdate)
					sys->Update(it.delta_time());
				sys->Run(ourPhase);
			});
	}

	for(auto phase : { flecs::PreUpdate }) {
		Phase ourPhase;
		if(phase == flecs::PreUpdate)
			ourPhase = Phase::PreUpdate;
		if(phase == flecs::OnUpdate)
			ourPhase = Phase::OnUpdate;
		if(phase == flecs::PostUpdate)
			ourPhase = Phase::PostUpdate;

		EntityWorld.GetNative()
		.system<ScriptComponent>()
		.kind(phase)
		.run(
			[&](flecs::iter& it)
			{
				auto sys = EntityWorld.Get<PhysicsSystem>();
				if(!sys)
					return;

				if(ourPhase == Phase::OnUpdate)
					sys->Update(it.delta_time());
				sys->Run(ourPhase);
			});
	}

	EntityWorld.GetNative()
	.observer()
	.with<RigidBodyComponent>()
	.event(flecs::Monitor)
	.each(
		[&](flecs::iter& it, size_t i)
		{
			auto sys = EntityWorld.Get<PhysicsSystem>();
			if(!sys)
				return;

			Entity entity{ it.entity(i) };

			if(it.event() == flecs::OnAdd)
				sys->OnComponentAdd(entity);
			else if(it.event() == flecs::OnSet)
				sys->OnComponentSet(entity);
			else if(it.event() == flecs::OnRemove)
				sys->OnComponentRemove(entity);
		});

	EntityWorld.GetNative()
	.observer()
	.with<ScriptComponent>()
	.event(flecs::Monitor)
	.each(
		[&](flecs::iter& it, size_t i)
		{
			auto sys = EntityWorld.Get<ScriptSystem>();
			if(!sys)
				return;

			Entity entity{ it.entity(i) };

			if(it.event() == flecs::OnAdd)
				sys->OnComponentAdd(entity);
			else if(it.event() == flecs::OnSet)
				sys->OnComponentSet(entity);
			else if(it.event() == flecs::OnRemove)
				sys->OnComponentRemove(entity);
		});
}

}