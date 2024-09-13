#pragma once

#include <string>

#define flecs_STATIC
#include <flecs.h>

#include <VolcaniCore/Core/Time.h>
#include <VolcaniCore/Core/UUID.h>

#include "Entity.h"

namespace Magma::ECS {

class World {
public:
	World();
	~World() = default;

	void OnUpdate(TimeStep ts);

	Entity GetEntity(const std::string& tag);

	Entity AddEntity();
	Entity AddEntity(VolcaniCore::UUID id);
	Entity AddEntity(const std::string& tag);

	void ForEach(const std::function<void(Entity&)>& func);

	template<typename TComponent>
	void ForEach(const std::function<void(Entity&)>& func) {
		flecs::query<TComponent>& query = GetQuery<TComponent>();

		query.each(
		[&func](flecs::entity handle) {
			Entity entity{ handle };
			func(entity);
		});
	}

	flecs::world& Get() { return m_World; }

private:
	flecs::world m_World;

	flecs::query<> m_AllEntitiesQuery;

	flecs::query<MeshComponent>		 m_MeshComponentQuery;
	flecs::query<RigidBodyComponent> m_RigidBodyComponentQuery;
	flecs::query<TagComponent>		 m_TagComponentQuery;
	flecs::query<TransformComponent> m_TransformComponentQuery;
	flecs::query<ScriptComponent>	 m_ScriptComponentQuery;

private:
	template<typename TComponent>
	flecs::query<TComponent>& GetQuery();
};

}