#pragma once

#include <flecs.h>

using namespace VolcaniCore;
using namespace Magma;

namespace Demo {

struct Position { float x, y; };
struct Velocity { float x, y; };

class ECS : public Application {
public:
	ECS();

	void OnUpdate(TimeStep ts);

private:
	flecs::world world;
};

ECS::ECS() {
	EventSystem::RegisterListener<KeyPressedEvent>(
	[](const KeyPressedEvent& event) {
		if(event.Key == Key::Escape)
			Application::Close();
	});

	auto e = world.entity("Bob");
	auto ent2 = world.lookup("Bob");
	e
	.set<Position>({10, 20})
	.set<Velocity>({1, 2});

	const Position *p = e.get<Position>(); // Returns nullptr if the component is missing
}

void ECS::OnUpdate(TimeStep ts) {

}

}