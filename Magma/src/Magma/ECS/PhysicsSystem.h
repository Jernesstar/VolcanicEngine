#pragma once

#include "Entity.h"

#include "Physics/World.h"

namespace Magma::ECS {

class PhysicsSystem {
public:
	static void Register(Physics::World& world, Entity& entity);

	static void RegisterForCollisionDetection(Entity& e1, Entity& e2);
	static bool Collides(Entity& e1, Entity& e2);
};

}