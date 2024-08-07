#pragma once

#include "ECS/Entity.h"

#include "Physics/World.h"

namespace Magma::ECS {

class PhysicsSystem {
public:
	static void Register(Physics::World& world, Entity& entity);

	static void RegisterForCollisionDetection(Physics::World& world,
											  Entity& e1, Entity& e2);
	static bool Collides(Entity& e1, Entity& e2);
};

}