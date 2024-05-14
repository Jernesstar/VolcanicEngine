#pragma once

#include <VolcaniCore/Core/Application.h>
#include <VolcaniCore/Core/Log.h>
#include <VolcaniCore/Events/EventSystem.h>

#include <Magma/Entity.h>
#include <Magma/SceneSerializer.h>

using namespace VolcaniCore;
using namespace Magma;

class ECSDemo : public Application {
public:
	ECSDemo();

	void OnUpdate(TimeStep ts);
};

ECSDemo::ECSDemo() {
	EventSystem::RegisterEventListener<KeyPressedEvent>(
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
	SceneSerializer::Serialize(scene, "Magma/temp.volc");

	Ref<Scene> scene2 = SceneSerializer::Deserialize("Magma/temp.volc");

	scene2->GetEntitySystem().ForEachEntity(
	[](Entity& entity) {
		VOLCANICORE_LOG_INFO("%s", entity.Get<TagComponent>().Tag.c_str());
	});
}

void ECSDemo::OnUpdate(TimeStep ts) {
	
}
