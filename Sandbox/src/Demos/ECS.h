#pragma once

#include <VolcaniCore/Core/Application.h>
#include <VolcaniCore/Core/Log.h>
#include <VolcaniCore/Events/EventSystem.h>
#include <Magma/Entity.h>

using namespace VolcaniCore;
using namespace Magma;

class ECSDemo : public Application {
public:
	ECSDemo();

	void OnUpdate(TimeStep ts);

private:
	Entity entity;
};

ECSDemo::ECSDemo() {
	EventSystem::RegisterEventListener<KeyPressedEvent>(
	[](const KeyPressedEvent& event) {
		if(event.Key == Key::Escape)
			Application::Close();
	});

	entity.Add<TransformComponent>();
	entity.Add<TextureComponent>("Sandbox/assets/images/apple.png");
	TagComponent& tag = entity.Add<TagComponent>("Test Entity");
}

void ECSDemo::OnUpdate(TimeStep ts) {
	
}
