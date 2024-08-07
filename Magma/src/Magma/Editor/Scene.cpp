#include "Scene.h"

#include <VolcaniCore/Renderer/Renderer3D.h>
#include <VolcaniCore/Renderer/ShaderLibrary.h>
#include <VolcaniCore/Renderer/StereographicCamera.h>

#include "ECS/PhysicsSystem.h"

using namespace Magma::ECS;
using namespace Magma::Physics;

namespace Magma {

Scene::Scene(const std::string& name)
	: Name(name)
{
	m_Camera = CreateRef<StereographicCamera>(75.0f);
	m_Controller = CreateRef<CameraController>(m_Camera);
	RegisterSystems();
	RegisterObservers();

	ShaderLibrary::Get("Mesh")->Bind();
}

void Scene::OnUpdate(TimeStep ts) {
	m_EntityWorld.OnUpdate(ts);
	m_Controller->OnUpdate(ts);
}

void Scene::OnRender() {
	Renderer3D::Begin(m_Camera);

	ShaderLibrary::Get("Mesh")->SetMat4("u_ViewProj", m_Camera->GetViewProjection());
	ShaderLibrary::Get("Mesh")->SetVec3("u_CameraPosition", m_Camera->GetPosition());
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
		auto mesh = m.Mesh;

		Material& material = mesh->GetMaterial();

		ShaderLibrary::Get("Mesh")->SetMat4("u_Model", tr);
		ShaderLibrary::Get("Mesh")->SetTexture("u_Diffuse", material.Diffuse, 0);
		Renderer3D::DrawMesh(mesh);
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
		// inherited the shape of the current mesh component

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