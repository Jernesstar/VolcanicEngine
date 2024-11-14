#pragma once

#include "Physics/World.h"

#include "System.h"

namespace Magma::ECS {

class PhysicsSystem : public System<RigidBodyComponent> {
public:
	void Submit(Entity& entity) override;
	void Update(TimeStep ts) override;
	void Run(Phase phase) override;

	void Register(Entity& entity);
	void Collides(Entity& e1, Entity& e2);
	bool Collided(Entity& e1, Entity& e2);

public:
	Physics::World& m_World;
};

}