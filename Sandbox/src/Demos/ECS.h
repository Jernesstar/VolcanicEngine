#pragma once

#include <Core/Application.h>
#include <Core/Log.h>
#include <Events/EventSystem.h>
#include <Renderer/OrthographicCamera.h>
#include <Renderer/StereographicCamera.h>

#include <Magma/Scene/Entity.h>
#include <Magma/Scene/SceneSerializer.h>

using namespace VolcaniCore;
using namespace Magma;

namespace Demo {

class ECS : public Application {
public:
	ECS();

	void OnUpdate(TimeStep ts);
};

ECS::ECS() {
	EventSystem::RegisterListener<KeyPressedEvent>(
	[](const KeyPressedEvent& event) {
		if(event.Key == Key::Escape)
			Application::Close();
	});

	Ref<Scene> scene = CreateRef<Scene>("Test Scene");
	scene->Camera = CreateRef<StereographicCamera>();

	Entity entity1, entity2, entity3;

	entity1.Add<TransformComponent>();
	entity1.Add<TextureComponent>("Sandbox/assets/images/apple.png");
	entity1.Add<TagComponent>("Entity1");

	entity2.Add<TransformComponent>();
	entity2.Add<TagComponent>("Entity2");
	entity2.Add<TextureComponent>("Sandbox/assets/images/start_bg.png");
	entity2.Add<EventListenerComponent>();

	scene->GetEntitySystem().AddEntity(entity1);
	scene->GetEntitySystem().AddEntity(entity2);
	scene->GetEntitySystem().AddEntity(entity3);
	SceneSerializer::Serialize(scene, "Sandbox/scenes/test.volc");

	scene.reset();
	scene = SceneSerializer::Deserialize("Sandbox/scenes/test.volc");

	scene->GetEntitySystem().ForEach(
	[](Entity& entity) {
		VOLCANICORE_LOG_INFO("%s", entity.Get<TagComponent>().Tag.c_str());
	});
}

void ECS::OnUpdate(TimeStep ts) {
	
}

}