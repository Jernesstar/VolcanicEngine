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
};

}