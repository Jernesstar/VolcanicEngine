#pragma once

#include "System.h"

namespace Magma::ECS {

class ScriptSystem : public System<ScriptComponent> {
public:
	void Submit(Entity& entity) override;
	void Update(TimeStep ts) override;
	void Run() override;
};

}