#pragma once

#include <flecs.h>

#include "Component.h"

namespace Magma::ECS {

class Entity {
public:
	Entity(flecs::entity handle)
		: m_Handle(handle) { }

	~Entity() = default;

	bool operator ==(const Entity& other) const {
		return this->m_Handle == other.m_Handle;
	}

	flecs::entity GetHandle() const { return m_Handle; }

	void Delete() {
		m_Handle.destruct();
	}

	template<typename TComponent>
	requires std::derived_from<TComponent, Component>
	bool Has() {
		return m_Handle.get_mut<TComponent>() != nullptr;
	}

	template<typename TComponent, typename ...Args>
	requires std::derived_from<TComponent, Component>
	TComponent& Add(Args&&... args) {
		m_Handle.set<TComponent>({ std::forward<Args>(args)... });
		return Get<TComponent>();
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

private:
	flecs::entity m_Handle;
};

}