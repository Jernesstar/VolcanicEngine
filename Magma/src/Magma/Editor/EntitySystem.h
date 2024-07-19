#pragma once

#include <flecs.h>

#include "Entity.h"

namespace Magma {

class EntitySystem {
public:
	EntitySystem() = default;
	~EntitySystem() = default;

	Entity& AddEntity(const Entity& entity) {

	}

	Entity& AddEntity(const std::string& tag) {
	
	}

	void RemoveEntity(Entity& entity) {
	
	}

	void ForEach(const std::function<void(Entity&)>& func) {
	
	}

private:
	flecs::world m_World;
};

}