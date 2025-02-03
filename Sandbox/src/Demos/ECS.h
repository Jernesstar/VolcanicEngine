#pragma once

using namespace Magma;

struct PlayerDiedEvent : public ECS::Event { };

namespace Demo {

class ECS : public Application {
public:
	ECS();

	void OnUpdate(TimeStep ts);

private:
	Ref<Scene> scene;
	// DefaultSceneRenderer renderer;
};

ECS::ECS() {
	Events::RegisterListener<KeyPressedEvent>(
		[](const KeyPressedEvent& event)
		{
			if(event.Key == Key::Escape)
				Application::Close();
		});

	scene = CreateRef<Scene>("Titled Scene");
	// renderer.GetCameraController().TranslationSpeed = 20.0f;

	auto& world = scene->EntityWorld;

	// world.Register<PlayerDiedEvent>();
	// world.Subscribe<PlayerDiedEvent>(
	// 	[](const PlayerDiedEvent& event)
	// 	{

	// 	});
	// world.Emit<PlayerDiedEvent>();

	EntityBuilder(world, "MainCamera")
	.Add<CameraComponent>(CreateRef<StereographicCamera>())
	.Finalize();

	auto player = Model::Create("Sandbox/assets/models/player/Knight_Golden_Male.obj");
	world.BuildEntity("Player")
	.Add<TransformComponent>(
		Transform
		{
			.Scale = glm::vec3(1.0f)
		})
	.Add<MeshComponent>(player)
	// .Add<RigidBodyComponent>(RigidBody::Type::Static)
	.Finalize();

	auto cube = Mesh::Create(MeshPrimitive::Cube,
		Material
		{
			.Diffuse = Texture::Create("Sandbox/assets/images/wood.png")
		});
	for(uint32_t i = 0; i < 20; i++)
		world.BuildEntity("Cube" + std::to_string(i + 1))
		.Add<TransformComponent>(
			Transform
			{
				.Translation = { (float)Random::RandInt(-10, 10), (float)Random::RandInt(-10, 10), (float)Random::RandInt(-10, 10) }
			})
		.Add<MeshComponent>(cube)
		// .Add<RigidBodyComponent>(RigidBody::Type::Static)
		.Finalize();

	auto ball = Mesh::Create("Sandbox/assets/models/sphere/wooden_sphere.obj");
	for(uint32_t i = 0; i < 20; i++)
		world.BuildEntity("Ball" + std::to_string(i + 1))
		.Add<TransformComponent>(
			Transform
			{
				.Translation = { (float)Random::RandInt(-10, 10), (float)Random::RandInt(-10, 10), (float)Random::RandInt(-10, 10) },
				.Scale = glm::vec3(0.1f)
			})
		.Add<MeshComponent>(ball)
		// .Add<RigidBodyComponent>(RigidBody::Type::Static)
		.Finalize();

	SceneLoader::Save(*scene, "Magma/assets/scenes/temp.magma.scene");
	VOLCANICORE_LOG_INFO("Success");
}

void ECS::OnUpdate(TimeStep ts) {
	// renderer.Update(ts);
	scene->OnUpdate(ts);
	// scene->OnRender(renderer);

	// Ref<Framebuffer> output = renderer.GetOutput();
	// Renderer2D::DrawFullscreenQuad(output, AttachmentTarget::Color);
}

}