#pragma once

#include "Physics/World.h"

#include "System.h"

namespace Magma::ECS {

class PhysicsSystem : public System<RigidBodyComponent> {
public:
	PhysicsSystem(World* world)
		: System(world) { }

	void Update(TimeStep ts) override;
	void Run(Phase phase) override;

	void Register(Entity& entity);
	void Collides(Entity& e1, Entity& e2);
	bool Collided(Entity& e1, Entity& e2);

	Physics::World& Get() { return m_World; }

public:
	Physics::World m_World;
};

}