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

	auto e = world.entity();
	VOLCANICORE_LOG_INFO("Alive: %d", e.is_alive());

	e.destruct();
	VOLCANICORE_LOG_INFO("Alive: %d", e.is_alive());

	auto entity = world.entity();
	
	// Add a component. This creates the component in the ECS storage, but does not
	// assign it with a value.
	entity.add<Velocity>();

	// Set the value for the Position & Velocity components. A component will be
	// added if the entity doesn't have it yet.
	entity.set<Position>({10, 20})
	.set<Velocity>({1, 2});

	// Get a component
	const Position *p = entity.get<Position>();
	
	// Remove component
	entity.remove<Position>();
}

void ECS::OnUpdate(TimeStep ts) {

}

}