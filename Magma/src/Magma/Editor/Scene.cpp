#include "Scene.h"

#include <VolcaniCore/Renderer/Renderer3D.h>

#include "ECS/PhysicsSystem.h"

using namespace Magma::ECS;
using namespace Magma::Physics;

namespace Magma {

Scene::Scene(const std::string& name)
	: Name(name)
{
	RegisterSystems();
	RegisterObservers();
}

void Scene::OnUpdate(TimeStep ts) {
	m_EntityWorld.OnUpdate(ts);
}

void Scene::OnRender() {
	if(!m_Camera)
		return;

	Renderer3D::Begin(m_Camera);

	m_RenderSys.run();

	Renderer3D::End();
}

void Scene::RegisterSystems() {
	auto& world = m_EntityWorld.Get();

	world
	.system<const ScriptComponent, TransformComponent>("InputUpdate")
	.kind(flecs::PreUpdate)
	.each(
	[](const ScriptComponent& s, TransformComponent& t) {
		s.OnInput(t);
	});
	world
	.system<const TransformComponent, RigidBodyComponent>("RigidBodyUpdate")
	.kind(flecs::PreUpdate)
	.each(
	[](const TransformComponent& t, RigidBodyComponent& r) {
		r.Body->UpdateTransform({ t.Translation, t.Rotation, t.Scale });
	});

	world
	.system("PhysicsUpdate")
	.kind(flecs::OnUpdate)
	.run(
	[&](flecs::iter& it) {
		m_PhysicsWorld.OnUpdate(it.delta_time());
	});

	world
	.system<const RigidBodyComponent, TransformComponent>("TransformUpdate")
	.kind(flecs::PostUpdate)
	.each(
	[](const RigidBodyComponent& r, TransformComponent& t) {
		Transform tr = r.Body->GetTransform();
		t.Translation = tr.Translation;
		t.Rotation	  = tr.Rotation;
		t.Scale		  = tr.Scale;
	});

	m_RenderSys = world
	.system<const TransformComponent, const MeshComponent>("RenderSystem")
	.kind(0)
	.each(
	[](const TransformComponent& t, const MeshComponent& m) {
		Transform tr{
			.Translation = t.Translation,
			.Rotation	 = t.Rotation,
			.Scale		 = t.Scale
		};

		Renderer3D::DrawMesh(m.Mesh, tr);
	});
}

void Scene::RegisterObservers() {
	auto world = m_EntityWorld.Get();

	// Creating RigidBodyComponent then MeshComponent ==> bounding volume
	// Creating MeshComponent then RigidBodyComponent ==> tightly-fitting volume

	world
	.observer<RigidBodyComponent>("OnSetRigidBody")
	.event(flecs::OnSet)
	.each(
	[&](flecs::entity e, RigidBodyComponent& r) {
		Entity entity{ e };

		// If the RigidBody was created without a shape,
		// inherit the shape of the current mesh component
		if(entity.Has<MeshComponent>() && !r.Body->HasShape()) {
			auto mesh = entity.Get<MeshComponent>().Mesh;
			Shape shape(mesh);
			r.Body->SetShape(shape);
		}
		if(entity.Has<TransformComponent>()) {
			auto& t = entity.Get<TransformComponent>();

			Transform tr{
				.Translation = t.Translation,
				.Rotation	 = t.Rotation,
				.Scale		 = t.Scale
			};
			r.Body->UpdateTransform(tr);
		}

		PhysicsSystem::Register(m_PhysicsWorld, entity);
	});
}

}