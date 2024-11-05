#include "Scene.h"

#include <VolcaniCore/Core/Assert.h>
#include <VolcaniCore/Renderer/Renderer3D.h>

#include "ECS/PhysicsSystem.h"

#include "SceneRenderer.h"

using namespace Magma::ECS;
using namespace Magma::Physics;

namespace Magma {

Scene::Scene(const std::string& name)
	: Name(name)
{
	RegisterSystems();
	RegisterObservers();

	m_Renderer = CreateRef<DefaultSceneRenderer>(this);
	m_Serializer = SceneSerializer{ this };
}

Scene::~Scene() {

}

void Scene::OnUpdate(TimeStep ts) {
	EntityWorld.OnUpdate(ts);
	m_Renderer->Update(ts);
}

void Scene::OnRender() {
	m_Renderer->Render();
}

void Scene::Load(const std::string& path) {
	m_Serializer.Deserialize(path);
}

void Scene::Save(const std::string& path) {
	m_Serializer.Serialize(path);
}

void Scene::RegisterSystems() {
	auto& world = EntityWorld.Get();

	world
	.system<const ScriptComponent, TransformComponent>("InputUpdate")
	.kind(flecs::PreUpdate)
	.each(
		[](const ScriptComponent& s, TransformComponent& t)
		{
			s.OnInput(t);
		});
	// world
	// .system<const TransformComponent, RigidBodyComponent>("RigidBodyUpdate")
	// .kind(flecs::PreUpdate)
	// .each(
	// [](const TransformComponent& t, RigidBodyComponent& r) {
	// 	r.Body->UpdateTransform({ t.Translation, t.Rotation, t.Scale });
	// });
	// world
	// .system("PhysicsUpdate")
	// .kind(flecs::OnUpdate)
	// .run(
	// [&](flecs::iter& it) {
	// 	PhysicsWorld.OnUpdate(it.delta_time());
	// });
	// world
	// .system<const RigidBodyComponent, TransformComponent>("TransformUpdate")
	// .kind(flecs::PostUpdate)
	// .each(
	// 	[](const RigidBodyComponent& rb, TransformComponent& tc)
	// 	{
	// 		Transform tr = rb.Body->GetTransform();
	// 		tc.Translation = tr.Translation;
	// 		tc.Rotation	   = tr.Rotation;
	// 		tc.Scale	   = tr.Scale;
	// 	});
}

void Scene::RegisterObservers() {
	auto& world = EntityWorld.Get();

	// Creating RigidBodyComponent then MeshComponent ==> bounding volume
	// Creating MeshComponent then RigidBodyComponent ==> tightly-fitting volume

	// world
	// .observer<RigidBodyComponent>("OnSetRigidBody")
	// .event(flecs::OnSet)
	// .each(
	// 	[&](flecs::entity e, RigidBodyComponent& r)
	// 	{
	// 		Entity entity{ e };

	// 		// If the RigidBody was created without a shape,
	// 		// inherit the shape of the current mesh component
	// 		if(entity.Has<MeshComponent>() && !r.Body->HasShape()) {
	// 			auto mesh = entity.Get<MeshComponent>().Mesh;
	// 			Ref<Shape> shape = Shape::Create(mesh);
	// 			r.Body->SetShape(shape);
	// 		}
	// 		if(entity.Has<TransformComponent>()) {
	// 			auto& t = entity.Get<TransformComponent>();

	// 			Transform tr{
	// 				.Translation = t.Translation,
	// 				.Rotation	 = t.Rotation,
	// 				.Scale		 = t.Scale
	// 			};
	// 			r.Body->UpdateTransform(tr);
	// 		}

	// 		PhysicsSystem::Register(PhysicsWorld, entity);
	// 	});
}

}