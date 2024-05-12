#pragma once

#include <functional>

#include "Entity.h"

namespace Magma {

class EntitySystem {
public:
	EntitySystem() = default;
	~EntitySystem() = default;

	Entity& AddEntity() { return m_Entities.emplace_back(); }
	Entity& AddEntity(const Entity& entity) { return m_Entities.emplace_back(entity); }

	void RemoveEntity(Entity& entity) {
		m_Entities.erase(std::remove(m_Entities.begin(), m_Entities.end(), entity), m_Entities.end());
	}

	void ForEachEntity(const std::function<void(Entity&)>& func) {
		for(auto& entity : m_Entities)
			func(entity);
	}

private:
	std::vector<Entity> m_Entities;
};

}