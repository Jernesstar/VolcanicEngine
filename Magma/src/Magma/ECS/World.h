#pragma once

#include <flecs/flecs.h>

#include <VolcaniCore/Core/Time.h>
#include <VolcaniCore/Core/Defines.h>
#include <VolcaniCore/Core/UUID.h>

#include "Entity.h"
#include "Component.h"
#include "System.h"

#include "EntityBuilder.h"

using namespace VolcaniCore;

namespace Magma::ECS {

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
	void Add(List<Phase> phases);

	template<typename TSystem>
	void Get();

	void ForEach(const Func<Entity&, void>& func);

	template<typename TComponent>
	void ForEach(const Func<Entity&, void>& func);

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