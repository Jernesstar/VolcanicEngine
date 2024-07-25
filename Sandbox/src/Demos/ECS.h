#pragma once

#include <flecs.h>

using namespace VolcaniCore;
using namespace Magma;

namespace Demo {

class ECS : public Application {
public:
	ECS();

	void OnUpdate(TimeStep ts);

private:
	Scene scene;
};

ECS::ECS() {
	EventSystem::RegisterListener<KeyPressedEvent>(
	[](const KeyPressedEvent& event) {
		if(event.Key == Key::Escape)
			Application::Close();
	});

	auto& world = scene.GetEntitySystem.GetWorld();

	Entity entity(world);
	entity
	.Add<TransformComponent>()
	.Add<MeshComponent>(MeshPrimitive::Cube,
		Material{
			.Diffuse = Texture::Create("Sandbox/assets/images/wood.png")
		});
}

void ECS::OnUpdate(TimeStep ts) {
	scene.OnUpdate(ts);
	scene.OnRender();
}

}