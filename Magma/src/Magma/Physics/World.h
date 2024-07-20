#pragma once

#include <PxPhysics.h>
#include <PxPhysicsAPI.h>

namespace Magma::Physics {

class World {
public:
	World();
	~World();

	void OnUpdate(TimeStep ts);

	// Sends a ray through the scene to see if it hits something
	// HitInfo Raycast(uint32_t x, uint32_t y);

private:
	PxScene* gScene;
};

}