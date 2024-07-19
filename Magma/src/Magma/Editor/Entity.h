#pragma once

#include "Component.h"

namespace Magma {

class Entity {
public:
	Entity(EntitySystem& world) {
		m_Entity = world.CreateEntity();
	}
	~Entity() {

	}
	
	UUID GetID() { return m_ID; }
	operator uint32_t() const { return m_ID; }

	template<typename TComponent>
	requires std::derived_from<TComponent, Component>
	bool Has() {

	}

	template<typename TComponent, typename ...Args>
	requires std::derived_from<TComponent, Component>
	TComponent& Add(Args&&... args) {

	}

	template<typename TComponent>
	requires std::derived_from<TComponent, Component>
	TComponent& Get() {

	}

	template<typename TComponent>
	requires std::derived_from<TComponent, Component>
	void Remove() {

	}

	bool operator ==(const Entity& other) const { 
		return this->m_ID == other.m_ID; 
	}

private:
	UUID m_ID;
};

}