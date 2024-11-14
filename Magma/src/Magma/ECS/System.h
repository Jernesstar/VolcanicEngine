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
	enum class RunStage { PreUpdate, OnUpdate, PostUpdate };

public:
	RunStage Stage;

public:
	System(RunStage stage)
		: Stage(stage) { }
	virtual ~System() = default;

	virtual void Submit(Entity& entity) { }
	virtual void Update(TimeStep ts) { }
	virtual void Run() { }

private:
	
};

}