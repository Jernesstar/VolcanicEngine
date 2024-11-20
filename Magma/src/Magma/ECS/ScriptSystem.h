#pragma once

#include "System.h"

namespace Magma::ECS {

class ScriptSystem : public System<ScriptComponent> {
public:
	ScriptSystem(World* world)
		: System(world) { }

	void Update(TimeStep ts) override;
	void Run(Phase phase) override;
};

}