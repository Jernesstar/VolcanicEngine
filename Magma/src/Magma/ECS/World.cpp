#include "World.h"

#include <VolcaniCore/Core/UUID.h>

#include "Scene/Component.h"

namespace Magma::ECS {

World::World() {
	m_AllEntitiesQuery = m_World.query_builder()
	.with<CameraComponent>().or_()
	.with<MeshComponent>().or_()
	.with<RigidBodyComponent>().or_()
	.with<TagComponent>().or_()
	.with<TransformComponent>().or_()
	.with<ScriptComponent>().or_()
	.with<DirectionalLightComponent>().or_()
	.with<PointLightComponent>().or_()
	.with<SpotlightComponent>()
	.build();

	m_EventHandler = m_World.entity("WorldEventHandler");
}

void World::OnUpdate(TimeStep ts) {
	m_World.progress(ts);
}

Entity World::GetEntity(const std::string& tag) {
	flecs::entity handle = m_World.lookup(tag.c_str());
	return Entity{ handle };
}

Entity World::GetEntity(VolcaniCore::UUID id) {
	flecs::entity handle = m_World.entity(id);
	return Entity{ handle };
}

Entity World::AddEntity() {
	flecs::entity handle = m_World.entity();
	return Entity{ handle };
}

Entity World::AddEntity(VolcaniCore::UUID id) {
	flecs::entity handle = m_World.make_alive((uint64_t)id);
	return Entity{ handle };
}

Entity World::AddEntity(const std::string& tag) {
	flecs::entity handle = m_World.entity(tag.c_str());
	Entity entity{ handle };
	entity.Add<TagComponent>(std::string(tag));

	return entity;
}

void World::RemoveEntity(VolcaniCore::UUID id) {
	GetEntity(id).Kill();
}

void World::RemoveEntity(const std::string& tag) {
	GetEntity(tag).Kill();
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