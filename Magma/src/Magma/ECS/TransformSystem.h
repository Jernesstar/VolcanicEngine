#pragma once

#include <VolcaniCore/Renderer/Transform.h>

#include "Entity.h"

using namespace VolcaniCore;

namespace Magma::ECS {

class TransformSystem {
public:
	static Transform GetTransform(Entity& entity);
	static void SetTransform(Entity& entity, const Transform& tr);

	static void Translate(Entity& entity);
	static void Rotate(Entity& entity);
	static void Scale(Entity& entity);
};

}