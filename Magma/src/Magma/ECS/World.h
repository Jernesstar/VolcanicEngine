#pragma once

#include <flecs/flecs.h>

#include <VolcaniCore/Core/Time.h>
#include <VolcaniCore/Core/Defines.h>
#include <VolcaniCore/Core/UUID.h>

#include "Entity.h"
#include "Component.h"
#include "System.h"

using namespace VolcaniCore;

namespace Magma::ECS {

class World {
public:
	World();
	~World() = default;

	void OnUpdate(TimeStep ts);

	Entity GetEntity(const std::string& tag);
	Entity GetEntity(VolcaniCore::UUID id);

	Entity AddEntity();
	Entity AddEntity(VolcaniCore::UUID id);
	Entity AddEntity(const std::string& tag);

	void RemoveEntity(VolcaniCore::UUID id);
	void RemoveEntity(const std::string& tag);

	template<typename TSystem>
	void Add(TSystem::RunStage stage) {
		Ref<System<>> sys = CreateRef<TSystem>();
		m_Systems.push_back(sys);

		m_World
		.system<TSystem::RequiredComponents>(m_Systems.size())
		.kind(stage)
		.run(
			[&](flecs::iter& it)
			{
				while(it.next())
					for(auto entityID : it) {
						Entity entity{ entityID };
						sys->Submit(entity);
					}

				sys->Update(it.delta_time());
				sys->Run();
			});
	}

	void ForEach(const Func<Entity&, void>& func);

	template<typename TComponent>
	void ForEach(const Func<Entity&, void>& func) {
		flecs::query<TComponent> query = GetQuery<TComponent>();

		query.each(
			[func](flecs::entity handle, TComponent& _)
			{
				Entity entity{ handle };
				func(entity);
			});
	}

	flecs::world& Get() { return m_World; }

private:
	flecs::world m_World;

	flecs::query<> m_AllEntitiesQuery;

	flecs::query<CameraComponent>	 m_CameraComponentQuery;
	flecs::query<MeshComponent>		 m_MeshComponentQuery;
	flecs::query<RigidBodyComponent> m_RigidBodyComponentQuery;
	flecs::query<TagComponent>		 m_TagComponentQuery;
	flecs::query<TransformComponent> m_TransformComponentQuery;
	flecs::query<ScriptComponent>	 m_ScriptComponentQuery;

	List<Ref<System<>>> m_Systems;

private:
	template<typename ...TComponents>
	flecs::query<TComponents...> GetQuery() {
		return m_World.query_builder<TComponents...>()
		.template with<TComponents...>()
		.cached()
		.build();
	}
};

}