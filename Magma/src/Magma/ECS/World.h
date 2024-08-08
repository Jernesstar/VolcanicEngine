#pragma once

#include <string>

#include <flecs.h>

#include <VolcaniCore/Core/Time.h>

#include "Entity.h"

namespace Magma::ECS {

class World {
public:
	World();
	~World() = default;

	void OnUpdate(TimeStep ts);

	Entity Get(const std::string& tag);

	Entity AddEntity();
	Entity AddEntity(VolcaniCore::UUID id);
	Entity AddEntity(const std::string& tag);

	void ForEach(const std::function<void(Entity& entity)>& func);

	// TODO(Implement): Queries
	flecs::world& Get() { return m_World; }

private:
	flecs::world m_World;

	flecs::query<> m_AllEntitiesQuery;
};

}