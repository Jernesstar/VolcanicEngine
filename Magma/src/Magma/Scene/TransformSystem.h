#pragma once

#include <VolcaniCore/Graphics/Transform.h>

#include "ECS/System.h"

#include "Component.h"

using namespace Magma::ECS;

namespace Magma {

class TransformSystem : public System<TransformComponent> {
public:
	TransformSystem(World* world)
		: System(world) { }

	void Update(TimeStep ts) override { }
	void Run(Phase phase) override { }

	Transform GetTransform(Entity& entity);
	void SetTransform(Entity& entity, const Transform& tr);

	void Translate(Entity& entity, const glm::vec3& vec);
	void Rotate(Entity& entity, const glm::vec3& vec);
	void Scale(Entity& entity, const glm::vec3& vec);
};

}