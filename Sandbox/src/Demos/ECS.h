#pragma once

namespace Demo {

class ECS : public Application {
public:
	ECS();

	void OnUpdate(TimeStep ts);

private:
	Ref<Scene> scene;
};

ECS::ECS() {
	EventSystem::RegisterListener<KeyPressedEvent>(
	[](const KeyPressedEvent& event) {
		if(event.Key == Key::Escape)
			Application::Close();
	});

	auto& world = scene->GetEntityWorld().Get();

	ECS::Entity entity = ECS::EntityBuilder(world)
	.Add<TransformComponent>()
	.Add<MeshComponent>(MeshPrimitive::Cube,
		Material{
			.Diffuse = Texture::Create("Sandbox/assets/images/wood.png")
		})
	.Add<RigidBodyComponent>(RigidBody::Type::Static)
	.Finalize();

	SceneSerializer::Serialize(scene, "Sandbox/assets/scenes/test.volc");
	Ref<Scene> test
		= SceneSerializer::Deserialize("Sandbox/assets/scenes/test.volc");
}

void ECS::OnUpdate(TimeStep ts) {
	scene.OnUpdate(ts);
	scene.OnRender();
}

}