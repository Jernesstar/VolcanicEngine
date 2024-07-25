#include "World.h"

#include <PxPhysics.h>
#include <PxPhysicsAPI.h>

using namespace physx;

namespace Magma::Physics {

World::World() {
	auto physicsLib = PhysicsSystem::s_Physics;
	PxSceneDesc sceneDesc(physicsLib->getTolerancesScale());
	sceneDesc.cpuDispatcher	= gDispatcher;
	sceneDesc.gravity		= PxVec3(0.0f, -9.81f, 0.0f);
	sceneDesc.filterShader	= PxDefaultSimulationFilterShader;
	sceneDesc.flags			= PxSceneFlag::eENABLE_ACTIVE_ACTORS;

	m_Scene = physicsLib->createScene(sceneDesc);
}

World::~World() {
	PX_RELEASE(m_Scene);
}

void World::OnUpdate(TimeStep ts) {
	m_Scene->simulate(1.0f/60.0f);
	// m_Scene->simulate((float)ts);
	m_Scene->fetchResults(true);
}

// HitInfo World::Raycast(uint32_t x, uint32_t y) {
	
// }

}