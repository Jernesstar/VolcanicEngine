#include "World.h"

namespace Magma::Physics {

World::World() {
	// PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	// sceneDesc.gravity		= PxVec3(0.0f, -9.81f, 0.0f);
	// sceneDesc.cpuDispatcher	= gDispatcher;
	// sceneDesc.filterShader	= PxDefaultSimulationFilterShader;
	// sceneDesc.flags			= PxSceneFlag::eENABLE_ACTIVE_ACTORS;

	// gScene = gPhysics->createScene(sceneDesc);
}

World::~World() {
	PX_RELEASE(gScene);
}

void World::OnUpdate(TimeStep ts) {
	gScene->simulate(1.0f/60.0f);
	// gScene->simulate((float)ts);
	gScene->fetchResults(true);
}

// HitInfo World::Raycast(uint32_t x, uint32_t y) {
	
// }

}