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
	auto& world = scene->EntityWorld;

	Entity cameraEntity = EntityBuilder(world, "MainCamera");
	cameraEntity.Add<CameraComponent>();

	Entity entity = EntityBuilder(world)
	.Add<TransformComponent>(Transform{ .Translation = { 0.0f, 0.0f, -3.0f } })
	.Add<MeshComponent>(MeshPrimitive::Cube,
		Material{
			.Diffuse = Texture::Create("Sandbox/assets/images/wood.png")
		})
	// .Add<RigidBodyComponent>(RigidBody::Type::Static)
	.Finalize();

	Entity ball = EntityBuilder(world)
	.Add<TransformComponent>(Transform{ .Translation = { 0.0f, 0.0f, -3.0f } })
	.Add<MeshComponent>("Sandbox/assets/models/sphere/wooden_sphere.obj")
	// .Add<RigidBodyComponent>(RigidBody::Type::Static)
	.Finalize();

	scene->Save("Sandbox/assets/scenes/test.volc");

	Ref<Scene> test = CreateRef<Scene>();
	test->Load("Sandbox/assets/scenes/test.volc");

	test->EntityWorld
	.ForEach<MeshComponent>(
		[](Entity& entity)
		{
			VOLCANICORE_LOG_INFO("Path: '%s'",
				entity.Get<MeshComponent>().Mesh->Path.c_str());
		});

	VOLCANICORE_LOG_INFO("Success");
}

void ECS::OnUpdate(TimeStep ts) {
	scene->OnUpdate(ts);
	scene->OnRender();
}

}