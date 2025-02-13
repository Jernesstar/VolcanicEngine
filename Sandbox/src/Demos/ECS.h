#pragma once

using namespace Magma;

struct PlayerDiedEvent : public ECS::Event { };

namespace Demo {

class ECS : public Application {
public:
	ECS();

	void OnUpdate(TimeStep ts);

private:
};

ECS::ECS() {
	Events::RegisterListener<KeyPressedEvent>(
		[](const KeyPressedEvent& event)
		{
			if(event.Key == Key::Escape)
				Application::Close();
		});

	auto scene = CreateRef<Scene>("Titled Scene");

	auto& world = scene->EntityWorld;

	// world.Register<PlayerDiedEvent>();
	// world.Subscribe<PlayerDiedEvent>(
	// 	[](const PlayerDiedEvent& event)
	// 	{

	// 	});
	// world.Emit<PlayerDiedEvent>();

}

void ECS::OnUpdate(TimeStep ts) {

}

}