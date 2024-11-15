#pragma once

#include "System.h"

namespace Magma::ECS {

class RenderSystem : public System<MeshComponent, TransformComponent> {
public:
	void Update(TimeStep ts) override;
	void Run(Phase phase) override;
};

}