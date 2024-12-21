#pragma once

#include <tuple>

#include <VolcaniCore/Core/Time.h>

#include "Entity.h"

using namespace VolcaniCore;

template<typename ...Args>
struct TypeListWrapper;

template<typename ...Args>
struct TypeListWrapper {
	using type = std::tuple<Args...>;
};

template<typename ...Args>
using TypeList = typename TypeListWrapper<Args...>::type;

namespace Magma::ECS {

enum Phase { PreUpdate, OnUpdate, PostUpdate };

class World;

template<typename ...TComponents>
class System {
public:
	using RequiredComponents = TypeList<TComponents...>;

public:
	System(World* world)
		: m_EntityWorld(world) { }
	virtual ~System() = default;

	virtual void Update(TimeStep ts) = 0;
	virtual void Run(Phase phase) = 0;

	TypeList<TComponents&...> GetRequired(Entity& entity) {
		return { entity.Get<TComponents>()... };
	}

protected:
	World* m_EntityWorld;

	flecs::query<TComponents&...> m_Query;
};

}