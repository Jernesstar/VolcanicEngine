#pragma once

#include "Physics/World.h"

#include "ECS/System.h"

#include "Component.h"

using namespace Magma::ECS;

namespace Magma {

struct PhysicsEvent {
	PhysicsEvent() = default;
	virtual ~PhysicsEvent() = default;
};

struct CollisionEvent : public PhysicsEvent {
	Entity Other;

	CollisionEvent(Entity entity)
		: Other(entity) { }
};

struct ClickedEvent : public PhysicsEvent {

};

class PhysicsSystem : public System<RigidBodyComponent> {
public:
	PhysicsSystem(ECS::World* world);

	void Update(TimeStep ts) override;
	void Run(Phase phase) override;

	void OnComponentAdd(Entity& entity) override;
	void OnComponentSet(Entity& entity) override;
	void OnComponentRemove(Entity& entity) override;

	void Collides(Entity& e1, Entity& e2);

	Physics::World& Get() { return m_World; }

public:
	Physics::World m_World;
};

}