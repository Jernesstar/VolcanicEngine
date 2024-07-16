#pragma once

#include <PxPhysicsAPI.h>

using namespace physx;

using namespace VolcaniCore;

namespace Demo {

class Physics : public Application {
public:
	Physics();

	void OnUpdate(TimeStep ts);

private:
	PxPhysics* pPhysicsSDK = NULL;
	PxFoundation* mFoundation;

	inline static PxDefaultErrorCallback gDefaultErrorCallback;
	inline static PxDefaultAllocator gDefaultAllocatorCallback;
};

Physics::Physics() {
	EventSystem::RegisterListener<KeyPressedEvent>(
	[](const KeyPressedEvent& event) {
		if(event.Key == Key::Escape)
			Application::Close();
	});

	mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);
	VOLCANICORE_ASSERT(mFoundation, "PxCreateFoundation failed!");
}

void Physics::OnUpdate(TimeStep ts) {

}

}