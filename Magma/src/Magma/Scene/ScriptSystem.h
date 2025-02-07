#pragma once

#include "ECS/System.h"

#include "Component.h"

using namespace Magma::ECS;

namespace Magma {

class ScriptSystem : public System<ScriptComponent> {
public:
	ScriptSystem(ECS::World* world)
		: System(world) { }

	void Update(TimeStep ts) override;
	void Run(Phase phase) override;

	void OnComponentAdd(Entity& entity) override;
	void OnComponentSet(Entity& entity) override;
	void OnComponentRemove(Entity& entity) override;

public:
	TimeStep m_TimeStep;
};

}