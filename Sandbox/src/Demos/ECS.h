#pragma once

using namespace Magma;

namespace Demo {

class ECS : public Application {
public:
	ECS();

	void OnUpdate(TimeStep ts);

private:
	Ref<Scene> scene;

	Ref<ShaderPipeline> shader;
	Ref<RenderPass> renderPass;
};

ECS::ECS() {
	Events::RegisterListener<KeyPressedEvent>(
	[](const KeyPressedEvent& event) {
		if(event.Key == Key::Escape)
			Application::Close();
	});

	scene = CreateRef<Scene>();
	scene->SetCamera(CreateRef<StereographicCamera>());
	auto& world = scene->GetEntityWorld();

	Magma::ECS::Entity entity = Magma::ECS::EntityBuilder(world)
	.Add<TransformComponent>(Transform{ .Translation = { 0.0f, 0.0f, -3.0f } })
	.Add<MeshComponent>(MeshPrimitive::Cube,
		Material{
			.Diffuse = Texture::Create("Sandbox/assets/images/wood.png")
		})
	.Add<RigidBodyComponent>(RigidBody::Type::Static)
	.Finalize();

	Magma::ECS::Entity ball = Magma::ECS::EntityBuilder(world)
	.Add<TransformComponent>(Transform{ .Translation = { 0.0f, 0.0f, -3.0f } })
	.Add<MeshComponent>("Sandbox/assets/models/sphere/wooden_sphere.obj")
	.Add<RigidBodyComponent>(RigidBody::Type::Static)
	.Finalize();

	SceneSerializer::Serialize(scene, "Sandbox/assets/scenes/test.volc");
	Ref<Scene> test
		= SceneSerializer::Deserialize("Sandbox/assets/scenes/test.volc");
	
	test->GetEntityWorld()
	.ForEach(
	[](Entity& entity) {
		VOLCANICORE_LOG_INFO("Path: '%s'", entity.Get<MeshComponent>().Mesh->Path.c_str());
	});

	VOLCANICORE_LOG_INFO("Success");

	shader = ShaderPipeline::Create({
		{ "VolcaniCore/assets/shaders/Mesh.glsl.vert", ShaderType::Vertex },
		{ "VolcaniCore/assets/shaders/Mesh.glsl.frag", ShaderType::Fragment }
	});

	renderPass = RenderPass::Create("Render Pass", shader);
}

void ECS::OnUpdate(TimeStep ts) {
	scene->OnUpdate(ts);

	Renderer::StartPass(renderPass);
	{
		Renderer::Clear();

		scene->OnRender();
	}
	Renderer::EndPass();
}

}