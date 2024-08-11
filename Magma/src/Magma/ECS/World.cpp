#include "World.h"

#include <VolcaniCore/Core/UUID.h>

#define GET_QUERY(TComponent) \
template<> \
flecs::query<TComponent>& World::GetQuery<TComponent>() { \
	return m_##TComponent##Query; \
}

namespace Magma::ECS {

GET_QUERY(MeshComponent)
GET_QUERY(RigidBodyComponent)
GET_QUERY(TagComponent)
GET_QUERY(TransformComponent)
GET_QUERY(ScriptComponent)

World::World() {
	m_AllEntitiesQuery = m_World.query_builder()
	.with<MeshComponent>().or_()
	.with<RigidBodyComponent>().or_()
	.with<TagComponent>().or_()
	.with<TransformComponent>().or_()
	.with<ScriptComponent>()
	.build();

	m_MeshComponentQuery = m_World.query_builder()
	.with<MeshComponent>()
	.build();
	m_RigidBodyComponentQuery = m_World.query_builder()
	.with<RigidBodyComponent>()
	.build();
	m_TagComponentQuery = m_World.query_builder()
	.with<TagComponent>()
	.build();
	m_TransformComponentQuery = m_World.query_builder()
	.with<TransformComponent>()
	.build();
	m_ScriptComponentQuery = m_World.query_builder()
	.with<ScriptComponent>()
	.build();
}

void World::OnUpdate(TimeStep ts) {
	m_World.progress(ts);
}

Entity World::GetEntity(const std::string& tag) {
	flecs::entity handle = m_World.lookup(tag.c_str());
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

void World::ForEach(const std::function<void(Entity& entity)>& func) {
	m_AllEntitiesQuery.each(
	[&func](flecs::entity handle) {
		Entity entity{ handle };
		func(entity);
	});
}

}