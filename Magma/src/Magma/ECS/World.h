#pragma once

#include <flecs/flecs.h>

#include <VolcaniCore/Core/Time.h>
#include <VolcaniCore/Core/Defines.h>
#include <VolcaniCore/Core/UUID.h>

#include "Core/TypeID.h"

#include "Entity.h"
#include "Component.h"
#include "System.h"

#include "EntityBuilder.h"

using namespace VolcaniCore;

namespace Magma::ECS {

template<typename>
struct strip;

template<typename ...T>
struct strip<std::tuple<T...>>
{
	using type = flecs::system_builder<T...>;
};

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

	template<typename TSystem>
	void Add(const List<Phase>& phases) {
		auto id = TypeIDGenerator<System<>>::GetID<TSystem>();
		if(!m_Systems.count(id))
			m_Systems[id] = new TSystem(this);

		auto sys = (TSystem*)m_Systems[id];
		using SystemType = strip<typename TSystem::RequiredComponents>::type;

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

	}

	template<typename TSystem>
	Ref<TSystem> Get() {
		auto id = TypeIDGenerator<System<>>::GetID<TSystem>();
		if(!m_Systems.count(id))
			Add<TSystem>({ Phase::OnUpdate });

		return Ref<TSystem>((TSystem*)m_Systems[id]);
	}

	void ForEach(const Func<void, Entity&>& func);

	template<typename TComponent>
	void ForEach(const Func<void, Entity&>& func) {
		flecs::query<TComponent> query = GetQuery<TComponent>();

		query.each(
			[func](flecs::entity handle, TComponent& _)
			{
				Entity entity{ handle };
				func(entity);
			});
	}

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

	flecs::world& GetNative() { return m_World; }

private:
	flecs::world m_World;

	flecs::query<> m_AllEntitiesQuery;

	flecs::query<CameraComponent>	 m_CameraComponentQuery;
	flecs::query<MeshComponent>		 m_MeshComponentQuery;
	flecs::query<RigidBodyComponent> m_RigidBodyComponentQuery;
	flecs::query<TagComponent>		 m_TagComponentQuery;
	flecs::query<TransformComponent> m_TransformComponentQuery;
	flecs::query<ScriptComponent>	 m_ScriptComponentQuery;

	Map<uint64_t, void*> m_Systems;

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