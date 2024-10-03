#pragma once

#include <flecs.h>

#include <VolcaniCore/Core/Time.h>
#include <VolcaniCore/Core/Defines.h>
#include <VolcaniCore/Core/UUID.h>

#include "Entity.h"

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

	void ForEach(const Func<Entity&, void>& func);

	template<typename TComponent>
	void ForEach(const Func<Entity&, void>& func) {
		flecs::query<TComponent>& query = GetQuery<TComponent>();

		query.each(
			[&func](flecs::entity handle)
			{
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