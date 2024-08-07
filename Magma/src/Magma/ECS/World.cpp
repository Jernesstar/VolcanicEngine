#include "World.h"

#include <VolcaniCore/Core/UUID.h>
#include <VolcaniCore/Core/Log.h>

namespace Magma::ECS {

World::World() {
	m_AllEntitiesQuery = m_World.query_builder()
	.with<MeshComponent>().or_()
	.with<RigidBodyComponent>().or_()
	.with<TagComponent>().or_()
	.with<TransformComponent>().or_()
	.with<ScriptComponent>()
	.build();
}

void World::OnUpdate(TimeStep ts) {
	m_World.progress(ts);
}

Entity World::Get(const std::string& tag) {
	return Entity{ m_World.lookup(tag.c_str()) };
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