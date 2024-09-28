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

	Ref<ShaderPipeline> shader;
	shader = ShaderPipeline::Create({
		{ "VolcaniCore/assets/shaders/Mesh.glsl.vert", ShaderType::Vertex },
		{ "VolcaniCore/assets/shaders/Mesh.glsl.frag", ShaderType::Fragment }
	});
	m_DrawPass = RenderPass::Create("Draw", shader);

	// PointLight light =
	// 	PointLight{ .Constant = 0.3f, .Linear = 0.0f, .Quadratic = 0.032f };
	// light.Position = { 0.0f, 2.0f, 0.0f };
	// light.Ambient  = { 0.2f, 0.2f, 0.2f };
	// light.Diffuse  = { 0.5f, 0.5f, 0.5f };
	// light.Specular = { 1.0f, 1.0f, 1.0f };

	// shader->Bind();

	// shader->SetInt("u_PointLightCount", 1);
	// shader->SetVec3("u_PointLights[0].Position", light.Position);
	// shader->SetVec3("u_PointLights[0].Ambient",  light.Ambient);
	// shader->SetVec3("u_PointLights[0].Diffuse",  light.Diffuse);
	// shader->SetVec3("u_PointLights[0].Specular", light.Specular);

	// shader->SetFloat("u_PointLights[0].Constant",  light.Constant);
	// shader->SetFloat("u_PointLights[0].Linear",    light.Linear);
	// shader->SetFloat("u_PointLights[0].Quadratic", light.Quadratic);

	// shader->SetTexture("u_Material.Diffuse", cube->GetMaterial().Diffuse, 0);
	// shader->SetTexture("u_Material.Specular", cube->GetMaterial().Specular, 1);
	// shader->SetFloat("u_Material.Shininess", 32.0f);
}

Scene::~Scene() {

}

void Scene::OnUpdate(TimeStep ts) {
	m_Controller.OnUpdate(ts);
	m_EntityWorld.OnUpdate(ts);
}

void Scene::OnRender() {
	Renderer::StartPass(m_DrawPass);
	{
		Renderer3D::Begin(m_Camera);

		m_RenderSystem.run();

		Renderer3D::End();
	}
	Renderer::EndPass();
	
	// if(m_Options.Bloom)
	// 	BloomPass();

}

void Scene::SetCamera(Ref<Camera> camera) {
	m_Camera = camera;
	m_Controller = CameraController{ camera };
}

void Scene::SetController(const CameraController& controller) {
	m_Controller = controller;
	m_Controller.SetCamera(m_Camera);
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
	// 	m_PhysicsWorld.OnUpdate(it.delta_time());
	// });

	world
	.system<const RigidBodyComponent, TransformComponent>("TransformUpdate")
	.kind(flecs::PostUpdate)
	.each(
		[](const RigidBodyComponent& r, TransformComponent& t)
		{
			Transform tr = r.Body->GetTransform();
			t.Translation = tr.Translation;
			t.Rotation	  = tr.Rotation;
			t.Scale		  = tr.Scale;
		});

	m_RenderSystem = world
	.system<const TransformComponent, const MeshComponent>("RenderSystem")
	.kind(0)
	.each(
		[](const TransformComponent& t, const MeshComponent& m)
		{
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

	// world
	// .observer<RigidBodyComponent>("OnSetRigidBody")
	// .event(flecs::OnSet)
	// .each(
	// [&](flecs::entity e, RigidBodyComponent& r) {
	// 	Entity entity{ e };

	// 	// If the RigidBody was created without a shape,
	// 	// inherit the shape of the current mesh component
	// 	if(entity.Has<MeshComponent>() && !r.Body->HasShape()) {
	// 		auto mesh = entity.Get<MeshComponent>().Mesh;
	// 		Ref<Shape> shape = Shape::Create(mesh);
	// 		r.Body->SetShape(shape);
	// 	}
	// 	if(entity.Has<TransformComponent>()) {
	// 		auto& t = entity.Get<TransformComponent>();

	// 		Transform tr{
	// 			.Translation = t.Translation,
	// 			.Rotation	 = t.Rotation,
	// 			.Scale		 = t.Scale
	// 		};
	// 		r.Body->UpdateTransform(tr);
	// 	}

	// 	PhysicsSystem::Register(m_PhysicsWorld, entity);
	// });
}

}