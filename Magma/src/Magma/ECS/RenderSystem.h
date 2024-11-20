#pragma once

#include "System.h"

namespace Magma::ECS {

class RenderSystem : public System<MeshComponent, TransformComponent> {
public:
	RenderSystem(World* world)
		: System(world) { }

	void Update(TimeStep ts) override;
	void Run(Phase phase) override;
};

}