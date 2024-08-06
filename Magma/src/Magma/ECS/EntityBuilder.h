#pragma once

#include "Entity.h"

namespace Magma::ECS {

class EntityBuilder {
public:
	EntityBuilder(ECS::World& world) {
		m_Handle = world.entity();
	}
	EntityBuilder(const std::string_view& tag) {
		m_Handle = world.entity(tag);
		Add<TagComponent>(tag);
	}
	~EntityBuilder() = default;

	template<typename TComponent, typename ...Args>
	requires std::derived_from<TComponent, Component>
	EntityBuilder& Add(Args&&... args) {
		m_Handle.set<TComponent>(TComponent(std::forward<Args>(args)...));
		return *this;
	}

	Entity Finalize() { return Entity{ m_Handle }; }

private:
	flecs::entity m_Handle;
};

}