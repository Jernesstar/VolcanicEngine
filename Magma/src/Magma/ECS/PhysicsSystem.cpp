#include "PhysicsSystem.h"

#include "World.h"

using namespace Magma::Physics;

namespace Magma::ECS {

struct CollidedWith { }; // Tag, indicates that a collision has occured

void PhysicsSystem::Update(TimeStep ts) {
	m_World.OnUpdate(ts);
}

void PhysicsSystem::Run(Phase phase) {
	if(phase == Phase::PreUpdate) {
		// m_EntityWorld->GetNative()
		// .query_builder(m_EntityWorld->GetNative(), m_Query)
		// .with<TransformComponent>()
		// .build()
		// .each(
		// 	[this](flecs::entity id)
		// 	{
		// 		Entity entity{ id };
		// 		auto [rc] = GetRequired(entity);
		// 		auto t = entity.Get<TransformComponent>();

		// 		rc.Body->UpdateTransform({ t.Translation, t.Rotation, t.Scale });
		// 	});
	}

	if(phase == Phase::OnUpdate) {
		
	}

	if(phase == Phase::PostUpdate) {
		// m_EntityWorld->GetNative()
		// .query_builder(m_EntityWorld->GetNative(), m_Query)
		// .build()
		// .each(
		// [this](flecs::entity id)
		// {
		// 	Entity entity{ id };
		// 	auto [rc] = GetRequired(entity);
		// 	auto& tc = entity.Get<TransformComponent>();

		// 	Transform tr = rc.Body->GetTransform();
		// 	tc.Translation = tr.Translation;
		// 	tc.Rotation	   = tr.Rotation;
		// 	tc.Scale	   = tr.Scale;
		// });
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

// void Scene::RegisterObservers() {
// 	auto& world = EntityWorld.GetNative();

// 	// Creating RigidBodyComponent then MeshComponent ==> bounding volume
// 	// Creating MeshComponent then RigidBodyComponent ==> tightly-fitting volume

// 	world
// 	.observer<RigidBodyComponent>("OnSetRigidBody")
// 	.event(flecs::OnSet)
// 	.each(
// 		[&](flecs::entity e, RigidBodyComponent& r)
// 		{
// 			Entity entity{ e };

// 			// If the RigidBody was created without a shape,
// 			// inherit the shape of the current MeshComponent
// 			if(entity.Has<MeshComponent>() && !r.Body->HasShape()) {
// 				auto mesh = entity.Get<MeshComponent>().Mesh;
// 				Ref<Shape> shape = Shape::Create(mesh);
// 				r.Body->SetShape(shape);
// 			}
// 			if(entity.Has<TransformComponent>()) {
// 				auto& t = entity.Get<TransformComponent>();

// 				Transform tr{
// 					.Translation = t.Translation,
// 					.Rotation	 = t.Rotation,
// 					.Scale		 = t.Scale
// 				};
// 				r.Body->UpdateTransform(tr);
// 			}

// 			Register(entity);
// 		});
// }

}