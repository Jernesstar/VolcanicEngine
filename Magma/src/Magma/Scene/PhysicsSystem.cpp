#include "PhysicsSystem.h"

#include "ECS/World.h"

#include <VolcaniCore/Core/Log.h>

using namespace Magma::Physics;

namespace Magma {

struct CollidedWith { }; // Tag, indicates that a collision has occured

PhysicsSystem::PhysicsSystem(ECS::World* world)
	: System(world)
{
}

void PhysicsSystem::Update(TimeStep ts) {
	m_World.OnUpdate(ts);
}

void PhysicsSystem::Run(Phase phase) {
	VOLCANICORE_LOG_INFO("%i", phase);
	if(phase == Phase::PreUpdate) {
		m_EntityWorld->ForEach<RigidBodyComponent, TransformComponent>(
			[this](Entity& entity)
			{
				auto [rc] = GetRequired(entity);
				auto t = entity.Get<TransformComponent>();

				rc.Body->UpdateTransform({ t.Translation, t.Rotation, t.Scale });
			});
	}

	if(phase == Phase::OnUpdate) {
		VOLCANICORE_LOG_INFO("Update");
	}

	if(phase == Phase::PostUpdate) {
		m_EntityWorld->ForEach<RigidBodyComponent, TransformComponent>(
			[this](Entity& entity)
			{
				auto [rc] = GetRequired(entity);
				auto& tc = entity.Get<TransformComponent>();

				Transform tr = rc.Body->GetTransform();
				tc.Translation = tr.Translation;
				tc.Rotation	   = tr.Rotation;
				tc.Scale	   = tr.Scale;
			});
	}
}

void PhysicsSystem::OnComponentAdd(Entity& entity) {
	VOLCANICORE_LOG_INFO("RigidBody added");
	// auto& r = entity.Get<RigidBodyComponent>();

	// Creating RigidBodyComponent then MeshComponent ==> bounding volume
	// Creating MeshComponent then RigidBodyComponent ==> tightly-fitting volume

	// If the RigidBody was created without a shape,
	// inherit the shape of the current MeshComponent
	// if(entity.Has<MeshComponent>() && !r.Body->HasShape()) {
	// 	auto mesh = entity.Get<MeshComponent>().Mesh;
	// 	Ref<Shape> shape = Shape::Create(mesh);
	// 	r.Body->SetShape(shape);
	// }
	// if(entity.Has<TransformComponent>()) {
	// 	auto& t = entity.Get<TransformComponent>();

	// 	Transform tr{
	// 		.Translation = t.Translation,
	// 		.Rotation	 = t.Rotation,
	// 		.Scale		 = t.Scale
	// 	};
	// 	r.Body->UpdateTransform(tr);
	// }

	// m_World.AddActor(r.Body);
}

void PhysicsSystem::OnComponentSet(Entity& entity) {

}

void PhysicsSystem::OnComponentRemove(Entity& entity) {

}

void PhysicsSystem::Collides(Entity& e1, Entity& e2) {
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