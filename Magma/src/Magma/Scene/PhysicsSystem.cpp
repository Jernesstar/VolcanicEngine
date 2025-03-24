#include "PhysicsSystem.h"

#include <VolcaniCore/Core/Log.h>

#include "ECS/World.h"

#include "Component.h"

using namespace Magma::Physics;

namespace Magma {

PhysicsSystem::PhysicsSystem(ECS::World* world)
	: System(world)
{
}

void PhysicsSystem::Update(TimeStep ts) {
	m_World.OnUpdate(ts);
}

void PhysicsSystem::Run(Phase phase) {
	if(phase == Phase::PreUpdate) {
		m_EntityWorld
		->ForEach<RigidBodyComponent, TransformComponent>(
			[this](Entity& entity)
			{
				auto [rc] = GetRequired(entity);
				auto t = entity.Get<TransformComponent>();
				rc.Body->SetTransform(t);
			});
	}
	if(phase == Phase::PostUpdate) {
		m_EntityWorld
		->ForEach<RigidBodyComponent, TransformComponent>(
			[this](Entity& entity)
			{
				auto [rc] = GetRequired(entity);
				auto& tc = entity.Set<TransformComponent>();
				tc = rc.Body->GetTransform();
			});
	}
}

void PhysicsSystem::OnComponentAdd(Entity& entity) {
	auto& rc = entity.Get<RigidBodyComponent>();
	rc.Body->Data = (void*)(uint64_t)entity.GetHandle();
	m_World.AddActor(rc.Body);

	m_World.AddContactCallback(
		[this](Ref<RigidBody> body1, Ref<RigidBody> body2)
		{
			Entity entity1 = m_EntityWorld->GetEntity((uint64_t)body1->Data);
			Entity entity2 = m_EntityWorld->GetEntity((uint64_t)body2->Data);
			if(entity1.Has<ScriptComponent>()) {
				auto obj = entity1.Get<ScriptComponent>().Instance;
				obj->Call("OnPhysicsEvent", CollisionEvent{ entity2 });
			}
		});
	// // Creating RigidBodyComponent then MeshComponent ==> bounding volume
	// // Creating MeshComponent then RigidBodyComponent ==> tightly-fitting volume

	// // If the RigidBody was created without a shape,
	// // inherit the shape of the current MeshComponent
	
	// if(entity.Has<MeshComponent>() && !r.Body->HasShape()) {
	// 	auto mesh = entity.Get<MeshComponent>().Mesh;
	// 	Ref<Shape> shape = Shape::Create(mesh);
	// 	r.Body->SetShape(shape);
	// }
	// if(entity.Has<TransformComponent>()) {
	// 	auto& t = entity.Get<TransformComponent>();
	// 	r.Body->UpdateTransform(t);
	// }
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

}

}