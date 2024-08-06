#include "World.h"

#include <VolcaniCore/Core/UUID.h>

namespace Magma::ECS {

void World::OnUpdate(TimeStep ts) {
	m_World.progress(ts);
}

Entity Get(const std::string& tag) {
	return Entity{ world.lookup(tag) };
}

Entity World::AddEntity() {
	flecs::entity handle = world.entity();
	return Entity{ handle };
}

Entity World::AddEntity(VolcaniCore::UUID id) {
	flecs::entity handle = world.make_alive((uint64_t)id);
	return Entity{ handle };
}

Entity World::AddEntity(const std::string& tag) {
	flecs::entity handle = world.entity(tag);
	Entity entity{ handle };
	entity.Add<TagComponent>(std::string(tag));

	return entity;
}

void World::ForEach(const std::function<void(Entity& entity)>& func) {
	m_World.each(
	[](flecs::entity handle) {
		Entity entity{ handle };
		func(entity);
	});
}

}