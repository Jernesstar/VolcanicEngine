#pragma once

using namespace Magma;

namespace Demo {

class ECS : public Application {
public:
	ECS();
	~ECS();

	void OnUpdate(TimeStep ts);

private:
	Ref<Scene> scene;
	Ref<UI::Image> image;
};

ECS::ECS() {
	Events::RegisterListener<KeyPressedEvent>(
		[](const KeyPressedEvent& event)
		{
			if(event.Key == Key::Escape)
				Application::Close();
		});

	scene = CreateRef<Scene>("Titled Scene");
	scene->GetRenderer().GetCameraController().TranslationSpeed = 20.0f;

	auto& world = scene->EntityWorld;

	EntityBuilder(world, "MainCamera")
	.Add<CameraComponent>()
	.Finalize();

	for(uint32_t i = 0; i < 10; i++)
		EntityBuilder(world, "Cube" + std::to_string(i + 1))
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
		EntityBuilder(world, "Ball" + std::to_string(i + 1))
		.Add<TransformComponent>(
			Transform
			{
				.Translation = { (float)Random::RandInt(-10, 10), (float)Random::RandInt(-10, 10), (float)Random::RandInt(-10, 10) },
				.Scale = glm::vec3(0.3f)
			})
		.Add<MeshComponent>("Sandbox/assets/models/sphere/wooden_sphere.obj")
		// .Add<RigidBodyComponent>(RigidBody::Type::Static)
		.Finalize();

	scene->Save("Magma/assets/scenes/temp.magma.scene");

	VOLCANICORE_LOG_INFO("Success");
	UI::Init();

	image = UI::Image::Create(
	{
		.Path = "Sandbox/assets/images/stone.png",
		.Width = 100,
		.Height = 100
	});

	// auto output = scene->GetRenderer().GetOutput();
	// image->SetImage(output, AttachmentTarget::Color);
}

ECS::~ECS() {
	UI::Close();
}

void ECS::OnUpdate(TimeStep ts) {
	RendererAPI::Get()->Clear();
	UI::Begin();

	scene->OnUpdate(ts);
	scene->OnRender();

	auto output = scene->GetRenderer().GetOutput();
	// RendererAPI::Get()->RenderFramebuffer(output, AttachmentTarget::Color);

	image->Render();

	UI::End();
}

}