#pragma once

using namespace Magma;

namespace Demo {

class ECS : public Application {
public:
	ECS();

	void OnUpdate(TimeStep ts);

private:
	Ref<Scene> scene;
};

ECS::ECS() {
	Events::RegisterListener<KeyPressedEvent>(
		[](const KeyPressedEvent& event)
		{
			if(event.Key == Key::Escape)
				Application::Close();
		});

	scene = CreateRef<Scene>();
	scene->GetRenderer().GetCameraController().TranslationSpeed = 20.0f;

	auto& world = scene->EntityWorld;

	EntityBuilder(world, "MainCamera")
	.Add<CameraComponent>()
	.Finalize();

	for(uint32_t i = 0; i < 10; i++)
		EntityBuilder(world, "Cube" + std::to_string(i))
		.Add<TransformComponent>(
			Transform
			{
				.Translation = { (float)Random::RandInt(-10, 10), (float)Random::RandInt(-10, 10), (float)Random::RandInt(-10, 10) }
			})
		.Add<MeshComponent>(MeshPrimitive::Cube,
			Material
			{
				.Diffuse = Texture::Create("Sandbox/assets/images/wood.png")
			})
		// .Add<RigidBodyComponent>(RigidBody::Type::Static)
		.Finalize();

	for(uint32_t i = 0; i < 10; i++)
		EntityBuilder(world, "Ball" + std::to_string(i))
		.Add<TransformComponent>(
			Transform
			{
				.Translation = { (float)Random::RandInt(-10, 10), (float)Random::RandInt(-10, 10), (float)Random::RandInt(-10, 10) },
				.Scale = glm::vec3(0.3f)
			})
		.Add<MeshComponent>("Sandbox/assets/models/sphere/wooden_sphere.obj")
		// .Add<RigidBodyComponent>(RigidBody::Type::Static)
		.Finalize();

	scene->Save("Magma/assets/scenes/temp.volc");

	VOLCANICORE_LOG_INFO("Success");
}

void ECS::OnUpdate(TimeStep ts) {
	scene->OnUpdate(ts);
	scene->OnRender();
	// VOLCANICORE_LOG_INFO("%i, %i", Random::RandInt(-2, 2), Random::RandInt(-2, 2));

	auto output = scene->GetRenderer().GetOutput();
	RendererAPI::Get()->RenderFramebuffer(output, AttachmentTarget::Color);
}

}