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
	Entity& AddEntity(const std::string& tag) {
		auto& entity = m_Entities.emplace_back();
		entity.Add<TagComponent>(tag);
		return entity;
	}

	void RemoveEntity(Entity& entity) {
		m_Entities.erase(std::remove(m_Entities.begin(), m_Entities.end(), entity), m_Entities.end());
	}

	void ForEach(const std::function<void(Entity&)>& func) {
		for(auto& entity : m_Entities)
			func(entity);
	}

private:
	std::vector<Entity> m_Entities;
};

}