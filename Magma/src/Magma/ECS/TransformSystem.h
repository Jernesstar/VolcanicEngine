#pragma once

#include <VolcaniCore/Renderer/Transform.h>

#include "System.h"

namespace Magma::ECS {

class TransformSystem : public System<TransformComponent> {
public:
	void Submit(Entity& entity) override;
	void Update(TimeStep ts) override;
	void Run() override;

	Transform GetTransform(Entity& entity);
	void SetTransform(Entity& entity, const Transform& tr);

	void Translate(Entity& entity);
	void Rotate(Entity& entity);
	void Scale(Entity& entity);
};

}