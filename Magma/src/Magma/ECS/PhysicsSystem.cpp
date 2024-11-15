#include "PhysicsSystem.h"

using namespace Magma::Physics;

namespace Magma::ECS {

struct CollidedWith { }; // Tag, indicates that a collision has occured

void PhysicsSystem::Update(TimeStep ts) {
	m_World.OnUpdate(ts);
}

void PhysicsSystem::Run(Phase phase) {
	if(phase == Phase::PreUpdate) {
		auto query =
		m_EntityWorld->query_builder(m_Query)
		.with<TransformComponent>()
		.each(
			[](flecs::entity id, RigidBodyComponent& r)
			{
				Entity entity{ id };
				auto t = entity.Get<TransformComponent>();
				r.Body->UpdateTransform({ t.Translation, t.Rotation, t.Scale });
			});
	}
}

void PhysicsSystem::Register(Entity& entity) {
	Ref<RigidBody> actor = entity.Get<RigidBodyComponent>().Body;

	m_World.AddActor(actor);
}

void PhysicsSystem::Collides(Entity& e1, Entity& e2)
{
	if(e1 == e2)
		return;

	auto actor1 = e1.Get<RigidBodyComponent>().Body;
	auto actor2 = e2.Get<RigidBodyComponent>().Body;
	auto handle1 = e1.GetHandle();
	auto handle2 = e2.GetHandle();

	m_World.AddContactCallback(
		actor1, actor2,
		[handle1, handle2](Ref<RigidBody>, Ref<RigidBody>)
		{
			handle1.add<CollidedWith>(handle2);
			handle2.add<CollidedWith>(handle1);
		});
}

bool PhysicsSystem::Collided(Entity& e1, Entity& e2) {
	auto handle1 = e1.GetHandle();
	auto handle2 = e2.GetHandle();

	return handle1.has<CollidedWith>(handle2);
}


}