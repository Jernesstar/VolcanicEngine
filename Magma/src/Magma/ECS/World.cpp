#include "World.h"

#include <VolcaniCore/Core/UUID.h>

#include "Scene/Component.h"

namespace Magma::ECS {

World::World() {
	// flecs::entity pipeline =
	// 	m_World.pipeline()
	// 		.with(flecs::System)
	// 		.up
	// 		.with(Phase::PreUpdate)
	// 		.with(Phase::OnUpdate)
	// 		.with(Phase::PostUpdate)
	// 		.build();

	// m_World.set_pipeline(pipeline);

	m_World.component<Component>();

	m_World.component<CameraComponent>();
	m_World.component<TagComponent>();
	m_World.component<TransformComponent>();
	m_World.component<AudioComponent>();
	m_World.component<MeshComponent>();
	m_World.component<SkyboxComponent>();
	m_World.component<ScriptComponent>();
	m_World.component<RigidBodyComponent>();
	m_World.component<DirectionalLightComponent>();
	m_World.component<PointLightComponent>();
	m_World.component<SpotlightComponent>();
	m_World.component<ParticleEmitterComponent>();

	m_AllEntitiesQuery = m_World.query_builder().with<Component>().build();
}

void World::Reset() {
	List<Entity> all;
	ForEach([&](Entity entity) { all.Add(entity); });
	for(auto entity : all)
		entity.Kill();
}

World& World::operator =(const World& other) {
	Reset();

	other.ForEach(
		[&](const Entity& entity)
		{
			Entity newEntity = AddEntity();
			if(entity.GetName() != "")
				newEntity.SetName(entity.GetName());

			if(entity.Has<CameraComponent>())
				newEntity.Add<CameraComponent>(entity.Get<CameraComponent>());
			if(entity.Has<TagComponent>())
				newEntity.Add<TagComponent>(entity.Get<TagComponent>());
			if(entity.Has<TransformComponent>())
				newEntity.Add<TransformComponent>(entity.Get<TransformComponent>());
			if(entity.Has<AudioComponent>())
				newEntity.Add<AudioComponent>(entity.Get<AudioComponent>());
			if(entity.Has<MeshComponent>())
				newEntity.Add<MeshComponent>(entity.Get<MeshComponent>());
			if(entity.Has<SkyboxComponent>())
				newEntity.Add<SkyboxComponent>(entity.Get<SkyboxComponent>());
			if(entity.Has<ScriptComponent>())
				newEntity.Add<ScriptComponent>(entity.Get<ScriptComponent>());
			if(entity.Has<RigidBodyComponent>())
				newEntity.Add<RigidBodyComponent>(entity.Get<RigidBodyComponent>());
			if(entity.Has<DirectionalLightComponent>())
				newEntity.Add<DirectionalLightComponent>(entity.Get<DirectionalLightComponent>());
			if(entity.Has<PointLightComponent>())
				newEntity.Add<PointLightComponent>(entity.Get<PointLightComponent>());
			if(entity.Has<SpotlightComponent>())
				newEntity.Add<SpotlightComponent>(entity.Get<SpotlightComponent>());
			if(entity.Has<ParticleEmitterComponent>()) {
				newEntity.Add<ParticleEmitterComponent>(entity.Get<ParticleEmitterComponent>());
				newEntity.GetHandle().modified<ParticleEmitterComponent>();
			}
		});

	return *this;
}

void World::OnUpdate(TimeStep ts) {
	m_World.progress(ts);
}

Entity World::GetEntity(const std::string& name) {
	flecs::entity handle = m_World.lookup(name.c_str());
	return Entity{ handle };
}

Entity World::GetEntity(VolcaniCore::UUID id) {
	flecs::entity handle = m_World.entity((uint64_t)id);
	return Entity{ handle };
}

Entity World::AddEntity() {
	flecs::entity handle = m_World.entity();
	handle.set<Component>({ });
	return Entity{ handle };
}

Entity World::AddEntity(VolcaniCore::UUID id) {
	flecs::entity handle = m_World.make_alive((uint64_t)id);
	handle.set<Component>({ });
	return Entity{ handle };
}

Entity World::AddEntity(const std::string& name) {
	flecs::entity handle = m_World.entity(name.c_str());
	handle.set<Component>({ });
	return Entity{ handle };
}

void World::RemoveEntity(VolcaniCore::UUID id) {
	GetEntity(id).Kill();
}

void World::RemoveEntity(const std::string& name) {
	GetEntity(name).Kill();
}

void World::ForEach(const Func<void, Entity&>& func) {
	m_World.defer_begin();

	m_AllEntitiesQuery.each(
		[&func](flecs::entity handle)
		{
			Entity entity{ handle };
			func(entity);
		});

	m_World.defer_end();
}

void World::ForEach(const Func<void, const Entity&>& func) const {
	m_World.defer_begin();

	m_AllEntitiesQuery.each(
		[&func](flecs::entity handle)
		{
			Entity entity{ handle };
			func(entity);
		});

	m_World.defer_end();
}

}