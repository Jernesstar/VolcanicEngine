#pragma once

#include <VolcaniCore/Core/Defines.h>
#include <VolcaniCore/Core/UUID.h>

#include "flecs/flecs.h"

#include "Entity.h"

namespace Magma::ECS {

class World;

class EntityBuilder {
public:
	EntityBuilder(const Entity& entity);
	EntityBuilder(World& world);
	EntityBuilder(World& world, VolcaniCore::UUID id);
	EntityBuilder(World& world, const std::string& tag);
	~EntityBuilder() = default;

	template<typename TComponent, typename ...Args>
	EntityBuilder& Add(Args&&... args) {
		m_Handle.set<TComponent>(TComponent(std::forward<Args>(args)...));
		return *this;
	}

	operator Entity() const { return Finalize(); }

	Entity Finalize() const { return Entity{ m_Handle }; }

private:
	flecs::entity m_Handle;
};

}