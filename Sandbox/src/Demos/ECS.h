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

private:
	Entity entity;
	Ref<Scene> scene;
};

ECSDemo::ECSDemo() {
	EventSystem::RegisterEventListener<KeyPressedEvent>(
	[](const KeyPressedEvent& event) {
		if(event.Key == Key::Escape)
			Application::Close();
	});

	scene = CreateRef<Scene>();
	scene->Camera = CreateRef<StereographicCamera>();

	entity.Add<TransformComponent>();
	entity.Add<TextureComponent>("Sandbox/assets/images/apple.png");
	TagComponent& tag = entity.Add<TagComponent>("Test Entity");

	scene->AddEntity(entity);
	SceneSerializer::Serialize(scene, "Magma/temp.volc");
}

void ECSDemo::OnUpdate(TimeStep ts) {
	
}
