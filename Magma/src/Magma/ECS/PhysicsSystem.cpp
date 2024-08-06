#include "PhysicsSystem.h"

namespace Magma::ECS {

struct Collided { }; // Tag, indicates that a collision has occured

void PhysicsSystem::Register(Physics::World& world, Entity& entity) {
	if(!entity.Has<RigidBodyComponent>())
		return;
	
	Ref<RigidBody> actor = entity.Get<RigidBodyComponent>().Body;

	world.AddActor(actor);
}

void PhysicsSystem::RegisterForCollisionDetection(Entity& e1, Entity& e2) {
	if(e1 == e2)
		return;
	if(!e1.Has<RigidBodyComponent>() || !e2.Has<RigidBodyComponent>())
		return;

	auto& actor1 = e1.Get<RigidBodyComponent>().Body;
	auto& actor2 = e2.Get<RigidBodyComponent>().Body;
	auto handle1 = e1.GetHandle();
	auto handle2 = e2.GetHandle();

	world.AddContactCallback(actor1, actor2
	[handle1, handle2](RigidBody& body1, RigidBody& body2) {
		handle1.add<Collided>(handle2);
		handle2.add<Collided>(handle1);
	});
}

bool PhysicsSystem::Collides(Entity& e1, Entity& e2) {
	auto handle1 = e1.GetHandle();
	auto handle2 = e2.GetHandle();

	return handle1.has<Collided>(handle2);
}


}