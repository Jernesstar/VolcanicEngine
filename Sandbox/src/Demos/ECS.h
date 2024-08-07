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
	EventSystem::RegisterListener<KeyPressedEvent>(
	[](const KeyPressedEvent& event) {
		if(event.Key == Key::Escape)
			Application::Close();
	});

	scene = CreateRef<Scene>();
	auto& world = scene->GetEntityWorld();

	Magma::ECS::Entity entity = Magma::ECS::EntityBuilder(world)
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
	
	test->GetEntityWorld()
	.ForEach(
	[](Entity& entity) {
		for(auto& v : entity.Get<MeshComponent>().Mesh->GetVertices())
			VOLCANICORE_LOG_INFO("Path: %f", v.TexCoord_Color.r);
	});

	VOLCANICORE_LOG_INFO("Success");
}

void ECS::OnUpdate(TimeStep ts) {
	scene->OnUpdate(ts);
	scene->OnRender();
}

}