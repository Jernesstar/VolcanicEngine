#pragma once

#include "ECS/World.h"

namespace Magma::ECS {

class EntityBuilder {
public:
	EntityBuilder(World& world) {
		m_Handle = world.AddEntity().GetHandle();
	}
	EntityBuilder(World& world, const std::string& tag) {
		m_Handle = world.AddEntity(tag).GetHandle();
	}
	~EntityBuilder() = default;

	template<typename TComponent, typename ...Args>
	requires std::derived_from<TComponent, Component>
	EntityBuilder& Add(Args&&... args) {
		m_Handle.set<TComponent>(TComponent(std::forward<Args>(args)...));
		return *this;
	}

	Entity Finalize() const { return Entity{ m_Handle }; }

private:
	flecs::entity m_Handle;
};

}