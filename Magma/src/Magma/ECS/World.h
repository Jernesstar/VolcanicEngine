#pragma once

#include <VolcaniCore/Core/Time.h>
#include <VolcaniCore/Core/Defines.h>
#include <VolcaniCore/Core/List.h>
#include <VolcaniCore/Core/UUID.h>
#include <VolcaniCore/Core/Log.h>

#include "Core/TypeID.h"

#include "Entity.h"
#include "EntityBuilder.h"
#include "System.h"

using namespace VolcaniCore;

namespace Magma::ECS {

struct Event { };

class World {
public:
	World();
	~World() = default;

	World& operator =(const World& other);

	void OnUpdate(TimeStep ts);

	Entity GetEntity(const std::string& name);
	Entity GetEntity(VolcaniCore::UUID id);

	EntityBuilder BuildEntity() { return EntityBuilder{ *this }; }
	EntityBuilder BuildEntity(VolcaniCore::UUID id) {
		return EntityBuilder{ *this, id };
	}
	EntityBuilder BuildEntity(const std::string& name) {
		return EntityBuilder{ *this, name };
	}

	Entity AddEntity();
	Entity AddEntity(VolcaniCore::UUID id);
	Entity AddEntity(const std::string& name);

	void RemoveEntity(VolcaniCore::UUID id);
	void RemoveEntity(const std::string& name);

	void ForEach(const Func<void, Entity&>& func);
	void ForEach(const Func<void, const Entity&>& func) const;

	template<typename ...TComponents>
	void ForEach(const Func<void, Entity&>& func) {
		flecs::query<TComponents...> q = 
			m_World.query_builder<TComponents...>().build();

		m_World.defer_begin();
		q.each(
			[func](flecs::entity handle, TComponents&...)
			{
				Entity entity{ handle };
				func(entity);
			});
		m_World.defer_end();
	}

	template<class TSystem>
	void Add() {
 		uint64_t id = TypeIDGenerator<System<>>::GetID<TSystem>();
		m_Systems[id] = new TSystem(this);
	}

	template<typename TSystem>
	TSystem* Get() {
		uint64_t id = TypeIDGenerator<System<>>::GetID<TSystem>();
		if (!m_Systems.size() || !m_Systems.count(id))
			return nullptr;

		return (TSystem*)m_Systems[id];
	}

	template<class TSystem>
	void Remove() {
 		uint64_t id = TypeIDGenerator<System<>>::GetID<TSystem>();
		if (!m_Systems.size() || !m_Systems.count(id))
			return;

		delete (TSystem*)m_Systems[id];
		m_Systems.erase(id);
	}

	flecs::world& GetNative() { return m_World; }

private:
	flecs::world m_World;
	flecs::query<> m_AllEntitiesQuery;
	Map<uint64_t, void*> m_Systems;
};

}