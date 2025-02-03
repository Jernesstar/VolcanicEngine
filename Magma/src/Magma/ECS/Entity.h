#pragma once

#include <flecs/flecs.h>

namespace Magma::ECS {

class Entity {
public:
	Entity() = default;
	Entity(flecs::entity handle)
		: m_Handle(handle) { }
	~Entity() = default;

	bool IsValid() const { return m_Handle != 0; }
	bool IsAlive() const { return m_Handle.is_alive(); }
	void Kill() {
		m_Handle.destruct();
	}

	template<typename TComponent>
	bool Has() const {
		return m_Handle.get<TComponent>() != nullptr;
	}

	template<typename TComponent, typename ...Args>
	TComponent& Add(Args&&... args) {
		m_Handle.set<TComponent>({ std::forward<Args>(args)... });
		return Get<TComponent>();
	}

	template<typename TComponent>
	TComponent& Get() {
		return *m_Handle.get_mut<TComponent>();
	}

	template<typename TComponent>
	const TComponent& Get() const {
		return *m_Handle.get<TComponent>();
	}

	template<typename TComponent>
	void Remove() {
		m_Handle.remove<TComponent>();
	}

	operator bool() const { return IsValid(); }

	bool operator ==(const Entity& other) const {
		return this->m_Handle == other.m_Handle;
	}

	flecs::entity GetHandle() const { return m_Handle; }

private:
	flecs::entity m_Handle;
};

}