#pragma once

#include <string>

#include <flecs.h>

#include <VolcaniCore/Core/Time.h>

#include "Entity.h"

namespace Magma::ECS {

class World {
public:
	World() = default;
	~World() = default;

	void OnUpdate(TimeStep ts);

	Entity Get(const std::string& tag);

	Entity AddEntity();
	Entity AddEntity(VolcaniCore::UUID id);
	Entity AddEntity(const std::string& tag);

	void ForEach(const std::function<void(Entity& entity)>& func);

	// TODO: Queries
	flecs::world& Get() { return m_World; }

private:
	flecs::world m_World;
};

}