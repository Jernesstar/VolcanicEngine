#pragma once

#include <flecs.h>

#include "Component.h"

namespace Magma {

class Entity {
public:
	Entity(flecs::world& world) {
		m_Handle = world.entity();
	}
	~Entity() {
		m_Handle.destruct();
	}

	template<typename TComponent>
	requires std::derived_from<TComponent, Component>
	bool Has() {
		return m_Handle.get_mut<TComponent>() != nullptr;
	}

	template<typename TComponent, typename ...Args>
	requires std::derived_from<TComponent, Component>
	Entity& Add(Args&&... args) {
		m_Handle.set<TComponent>(TComponent(std::forward<Args>(args)...));
		return *this;
	}

	template<typename TComponent>
	requires std::derived_from<TComponent, Component>
	TComponent& Get() {
		return *m_Handle.get_mut<TComponent>();
	}

	template<typename TComponent>
	requires std::derived_from<TComponent, Component>
	void Remove() {
		m_Handle.remove<TComponent>();
	}

	bool operator ==(const Entity& other) const {
		return this->m_Handle == other.m_Handle;
	}

private:
	flecs::entity m_Handle;
};

}