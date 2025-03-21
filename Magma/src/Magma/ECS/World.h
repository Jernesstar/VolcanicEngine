#pragma once

#include <flecs.h>

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

	// template<typename TEvent>
	// void Register() {
	// 	m_EventHandler.observe<TEvent>(
	// 		[this](TEvent& event) {
	// 			auto& list =
	// 				m_Events[TypeIDGenerator<Event>::GetID<TEvent>()];
	// 			for(auto& callback : list)
	// 				((Func<void, const TEvent&>)callback)(event);
	// 		});
	// }

	// template<typename TEvent>
	// void Subscribe(const Func<void, const TEvent&>& callback) {
	// 	m_Events[TypeIDGenerator<Event>::GetID<TEvent>()]
	// 		.push_back((Func<void, const Event&>)callback);
	// }

	// template<typename TEvent, typename ...Args>
	// void Emit(Args&&... args) {
	// 	m_EventHandler.emit<TEvent>(TEvent{ std::forward<Args>(args)... });
	// }

	template<class TSystem>
	void Add() {
 		uint64_t id = TypeIDGenerator<System<>>::GetID<TSystem>();
		m_Systems[id] = new TSystem(this);
	}

	template<typename TSystem>
	TSystem* Get() {
		auto id = TypeIDGenerator<System<>>::GetID<TSystem>();
		if(!m_Systems.count(id))
			return nullptr;

		return (TSystem*)m_Systems[id];
	}

	template<class TSystem>
	void Remove() {
 		uint64_t id = TypeIDGenerator<System<>>::GetID<TSystem>();
		if(!m_Systems.count(id))
			return;

		delete (TSystem*)m_Systems[id];
		m_Systems.erase(id);
	}

	flecs::world& GetNative() { return m_World; }

private:
	flecs::world m_World;

	flecs::query<> m_AllEntitiesQuery;

	flecs::entity m_EventHandler;

	Map<uint64_t, void*> m_Systems;
	Map<uint64_t, List<Func<void, const Event&>>> m_Events;
};

}