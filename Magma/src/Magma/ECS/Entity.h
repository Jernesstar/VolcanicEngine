#pragma once

#ifdef _MSC_VER
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#endif

#include <string>

#include <flecs.h>

namespace Magma::ECS {

class Entity {
public:
	Entity() = default;
	Entity(flecs::entity handle)
		: m_Handle(handle) { }
	~Entity() = default;

	bool IsValid() const { return m_Handle.is_valid(); }
	bool IsAlive() const { return m_Handle.is_alive(); }
	void Kill() {
		m_Handle.destruct();
	}

	template<typename TComponent>
	bool Has() const {
		return m_Handle.get<TComponent>() != nullptr;
	}

	template<typename TComponent, typename ...Args>
	void Add(Args&&... args) {
		if(!Has<TComponent>())
			m_Handle.emplace<TComponent>(std::forward<Args>(args)...);
	}

	template<typename TComponent>
	const TComponent& Get() const {
		return *m_Handle.get<TComponent>();
	}

	template<typename TComponent>
	TComponent& Set() {
		if(!Has<TComponent>())
			Add<TComponent>();
		// else
		// 	m_Handle.modified<TComponent>();
		return *m_Handle.get_mut<TComponent>();
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

	std::string GetName() const {
		auto name = m_Handle.name();
		if(name.length())
			return m_Handle.name().c_str();
		return "";
	}

	void SetName(const std::string& name) {
		m_Handle.set_name(name.c_str());
	}

private:
	flecs::entity m_Handle;
};

}