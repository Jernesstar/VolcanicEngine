#pragma once

#include <PxPhysics.h>
#include <PxPhysicsAPI.h>

using namespace physx;

#include <VolcaniCore/Core/Time.h>

using namespace VolcaniCore;

namespace Magma::Physics {

class World {
public:
	World();
	~World();

	void OnUpdate(TimeStep ts);

	// void AddActor(Ref<RigidBody> body);

	// Sends a ray through the scene to see if it hits something
	// HitInfo Raycast(uint32_t x, uint32_t y);

private:
	PxScene* m_Scene;
};

}