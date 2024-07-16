#pragma once

#include <PxPhysicsAPI.h>

using namespace VolcaniCore;

namespace Demo {

class Physics : public Application {
public:
	Physics();

	void OnUpdate(TimeStep ts);

private:
	physx::PxPhysics* pPhysicsSDK = NULL;
};

Physics::Physics() {
	EventSystem::RegisterListener<KeyPressedEvent>(
	[](const KeyPressedEvent& event) {
		if(event.Key == Key::Escape)
			Application::Close();
	});
}

void Physics::OnUpdate(TimeStep ts) {

}

}