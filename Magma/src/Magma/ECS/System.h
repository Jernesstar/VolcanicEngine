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

template<typename ...TRequires>
class System {
	using RequiredComponents = TypeList<TRequires...>;

public:
	enum class Phase { PreUpdate, OnUpdate, PostUpdate };

public:
	System(Phase stage)
		: Stage(stage) { }
	virtual ~System() = default;

	virtual void Submit(Entity& entity) = 0;
	virtual void Update(TimeStep ts) = 0;
	virtual void Run(Phase phase) = 0;

	RequiredComponents& GetRequired(Entity& entity) {
		return entity.Get<TRequires>()...;
	}

private:
	
};

}