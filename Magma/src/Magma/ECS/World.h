#pragma once

#include <flecs/flecs.h>

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

template<typename>
struct stripSystem;

template<typename ...T>
struct stripSystem<std::tuple<T...>>
{
	using type = flecs::system_builder<T...>;
};

// template<typename>
// struct stripObserver;

// template<typename ...T>
// struct stripObserver<std::tuple<T...>>
// {
// 	using type = flecs::observer<T...>;
// };

struct Event { };

class World {
public:
	World();
	~World() = default;

	void OnUpdate(TimeStep ts);

	Entity GetEntity(const std::string& tag);
	Entity GetEntity(VolcaniCore::UUID id);

	EntityBuilder BuildEntity() { return EntityBuilder{ *this }; }
	EntityBuilder BuildEntity(VolcaniCore::UUID id) {
		return EntityBuilder{ *this, id };
	}
	EntityBuilder BuildEntity(const std::string& tag) {
		return EntityBuilder{ *this, tag };
	}

	Entity AddEntity();
	Entity AddEntity(VolcaniCore::UUID id);
	Entity AddEntity(const std::string& tag);

	void RemoveEntity(VolcaniCore::UUID id);
	void RemoveEntity(const std::string& tag);

	void ForEach(const Func<void, Entity&>& func);

	void ForEach(const Func<void, const Entity&>& func) const;

	template<typename ...TComponents>
	void ForEach(const Func<void, Entity&>& func) {
		flecs::query<TComponents...> query = GetQuery<TComponents...>();

		query.each(
			[func](flecs::entity handle, TComponents&...)
			{
				Entity entity{ handle };
				func(entity);
			});
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

	template<typename TSystem>
	void Add(const List<Phase>& phases) {
		using SystemType =
			stripSystem<typename TSystem::RequiredComponents>::type;
		// using ObserverType =
		// 	stripObserver<typename TSystem::RequiredComponents>::type;

 		uint64_t id = TypeIDGenerator<System<>>::GetID<TSystem>();
		if(!m_Systems.count(id))
			m_Systems[id] = new TSystem(this);

		auto sys = (TSystem*)m_Systems[id];

		for(const auto& phase : phases)
			SystemType(m_World)
			.kind(phase)
			.run(
				[sys = sys, phase = phase](flecs::iter& it)
				{
					if(phase == Phase::OnUpdate)
						sys->Update(it.delta_time());
					sys->Run(phase);
				});

		// ObserverType(m_World)
		// .event(flecs::OnSet)
		// .each(
		// 	[&](flecs::entity e, RigidBodyComponent& r)
	}

	template<typename TSystem>
	TSystem* Get() {
		auto id = TypeIDGenerator<System<>>::GetID<TSystem>();
		if(!m_Systems.count(id))
			Add<TSystem>({ Phase::OnUpdate });

		return (TSystem*)m_Systems[id];
	}

	flecs::world& GetNative() { return m_World; }

private:
	flecs::world m_World;

	flecs::query<> m_AllEntitiesQuery;

	flecs::entity m_EventHandler;

	Map<uint64_t, void*> m_Systems;
	Map<uint64_t, List<Func<void, const Event&>>> m_Events;

private:
	template<typename ...TComponents>
	flecs::query<TComponents...> GetQuery() {
		return m_World.query_builder<TComponents...>()
		.template with<TComponents...>()
		// .cached()
		.build();
	}
};

}