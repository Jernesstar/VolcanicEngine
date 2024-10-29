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

	auto cube = Mesh::Create(MeshPrimitive::Cube,
		Material
		{
			.Diffuse = Texture::Create("Sandbox/assets/images/wood.png")
		});
	for(uint32_t i = 0; i < 12; i++)
		EntityBuilder(world, "Cube" + std::to_string(i + 1))
		.Add<TransformComponent>(
			Transform
			{
				.Translation = { (float)Random::RandInt(-5, 5), (float)Random::RandInt(-5, 5), (float)Random::RandInt(-5, 5) }
			})
		.Add<MeshComponent>(cube)
		// .Add<RigidBodyComponent>(RigidBody::Type::Static)
		.Finalize();

	auto ball = Mesh::Create("Sandbox/assets/models/sphere/wooden_sphere.obj");
	for(uint32_t i = 0; i < 12; i++)
		EntityBuilder(world, "Ball" + std::to_string(i + 1))
		.Add<TransformComponent>(
			Transform
			{
				.Translation = { (float)Random::RandInt(-5, 5), (float)Random::RandInt(-5, 5), (float)Random::RandInt(-5, 5) },
				.Scale = glm::vec3(0.1f)
			})
		.Add<MeshComponent>(ball)
		// .Add<RigidBodyComponent>(RigidBody::Type::Static)
		.Finalize();

	// scene->Save("Magma/assets/scenes/temp.magma.scene");

	UI::Init();

	image = UI::Image::Create(
	{
		.Path = "Sandbox/assets/images/stone.png",
		.Width = 100,
		.Height = 100
	});

	auto output = scene->GetRenderer().GetOutput();
	image->SetImage(output, AttachmentTarget::Color);
	VOLCANICORE_LOG_INFO("Success");
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

	image->SetSize(400, 400);
	image->Render();

	UI::End();
}

}