#include "World.h"

#include <VolcaniCore/Core/Application.h>

#include "Physics.h"

using namespace physx;

namespace Magma::Physics {

// TODO:
World::World() {
	PxSceneDesc sceneDesc(Physics::GetPhysicsLib()->getTolerancesScale());
	sceneDesc.cpuDispatcher	= Physics::GetDispatcher();
	sceneDesc.gravity		= PxVec3(0.0f, -9.81f, 0.0f);
	sceneDesc.filterShader	= PxDefaultSimulationFilterShader;
	// sceneDesc.filterShader	= FilterShaderExample;
	sceneDesc.simulationEventCallback = &m_ContactCallback;
	// sceneDesc.flags			= PxSceneFlag::eENABLE_ACTIVE_ACTORS;
	m_Scene = GetPhysicsLib()->createScene(sceneDesc);
}

World::~World() {
	PX_RELEASE(m_Scene);
}

void World::OnUpdate(TimeStep ts) {
	m_Accumulator += (float)ts;
	if(m_Accumulator < StepSize)
		return;

	m_Accumulator -= StepSize;

	m_Scene->simulate(StepSize);
	m_Scene->fetchResults(true);
}

void World::AddActor(Ref<RigidBody> body) {
	m_Scene->addActor(*body->m_Actor);
	m_Actors.push_back(body);
	m_ActorCount++;
}

HitInfo World::Raycast(const glm::vec3& start,
					   const glm::vec3& direction, float maxDist)
{
	glm::vec3 rayDir = glm::normalize(direction);
	PxHitFlags flags = PxHitFlag::ePOSITION;
					//  | PxHitFlag::eNORMAL
					//  | PxHitFlag::eUV;
	// PxReal maxDist = 10.0e+5;
	PxRaycastBuffer hitInfo;
	PxVec3 st{ start.x, start.y, start.z };
	PxVec3 dir{ rayDir.x, rayDir.y, rayDir.z };

	bool hit = m_Scene->raycast(st, dir, maxDist, hitInfo, flags);
	if(hit)
		return HitInfo((RigidBody*)hitInfo.block.actor->userData,
					   hitInfo.block.distance);
	else
		return HitInfo();
}

void World::AddContactCallback(
	const std::function<void(RigidBody&, RigidBody&)>& callback)
{
	m_ContactCallback.AddCallback(callback);
}

void ContactCallback::onTrigger(PxTriggerPair* pairs, PxU32 count)
{
	// for(PxU32 i=0; i<count; i++)
	// {
	// 	ignore pairs when shapes have been deleted
	// 	if(pairs[i].flags & (PxTriggerPairFlag::eREMOVED_SHAPE_TRIGGER | PxTriggerPairFlag::eREMOVED_SHAPE_OTHER))
	// 		continue;

	// 	Detect for example that a player entered a checkpoint zone
	// 	if((&pairs[i].otherShape->getActor() == gPlayerActor) &&
	// 		(&pairs[i].triggerShape->getActor() == gSensorActor))
	// 	{
	// 		gCheckpointReached = true;
	// 	}
	// }
}

void ContactCallback::onContact(const PxContactPairHeader& pairHeader,
								const PxContactPair* pairs, PxU32 nbPairs)
{
	RigidBody& a1 = *static_cast<RigidBody*>(pairHeader.actors[0]->userData);
	RigidBody& a2 = *static_cast<RigidBody*>(pairHeader.actors[1]->userData);

	for(auto callback : m_Callbacks) {
		callback(a1, a2);
	}
}

}