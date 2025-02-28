#include "World.h"

#include "Physics.h"

namespace Magma::Physics {

const float World::StepSize = 1.0f / 60.0f;

World::World() {
#ifdef MAGMA_PHYSICS
	PxSceneDesc sceneDesc(Physics::GetPhysicsLib()->getTolerancesScale());
	sceneDesc.cpuDispatcher	= Physics::GetDispatcher();
	sceneDesc.gravity		= PxVec3(0.0f, -9.81f, 0.0f);
	sceneDesc.filterShader	= PxDefaultSimulationFilterShader;
	// sceneDesc.filterShader	= FilterShaderExample;
	sceneDesc.simulationEventCallback = &m_ContactCallback;
	// sceneDesc.flags			= PxSceneFlag::eENABLE_ACTIVE_ACTORS;
	m_Scene = GetPhysicsLib()->createScene(sceneDesc);
#endif
}

World::~World() {
#ifdef MAGMA_PHYSICS
	PX_RELEASE(m_Scene);
#endif
}

void World::OnUpdate(TimeStep ts) {
#ifdef MAGMA_PHYSICS
	m_Accumulator += (float)ts;
	if(m_Accumulator < StepSize)
		return;

	m_Accumulator -= StepSize;

	m_Scene->simulate(StepSize);
	m_Scene->fetchResults(true);
#endif
}

void World::AddActor(Ref<RigidBody> body) {
#ifdef MAGMA_PHYSICS
	m_Scene->addActor(*body->m_Actor);
	m_Actors.Add(body);
	m_ActorCount++;
#endif
}

HitInfo World::Raycast(const glm::vec3& start, const glm::vec3& direction,
						float maxDist)
{
#ifdef MAGMA_PHYSICS
	glm::vec3 rayDir = glm::normalize(direction);
	PxHitFlags flags = PxHitFlag::ePOSITION;
					//  | PxHitFlag::eNORMAL
					//  | PxHitFlag::eUV;
	// PxReal maxDist = 10.0e+5;
	PxVec3 st{ start.x, start.y, start.z };
	PxVec3 dir{ rayDir.x, rayDir.y, rayDir.z };
	PxRaycastBuffer hitInfo;

	bool hasHit = m_Scene->raycast(st, dir, maxDist, hitInfo, flags);
	if(hasHit) {
		auto hit = hitInfo.block;
		RigidBody* actor = static_cast<RigidBody*>(hit.actor->userData);

		return HitInfo(actor, hit.distance);
	}
#endif

	return HitInfo();
}

void World::AddContactCallback(
	const std::function<void(Ref<RigidBody>, Ref<RigidBody>)>& callback)
{
#ifdef MAGMA_PHYSICS
	m_ContactCallback.AddCallback(callback);
#endif
}

void World::AddContactCallback(Ref<RigidBody> actor1, Ref<RigidBody> actor2,
	const std::function<void(Ref<RigidBody>, Ref<RigidBody>)>& callback)
{
#ifdef MAGMA_PHYSICS
	m_ContactCallback.AddCallback(
	[callback, actor1, actor2]
	(Ref<RigidBody> body1, Ref<RigidBody> body2) {
		if(((*body1 == *actor1) && (*body2 == *actor2))
		|| ((*body1 == *actor2) && (*body2 == *actor1)))
		{
			callback(body1, body2);
		}
	});
#endif
}

#ifdef MAGMA_PHYSICS
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
	RigidBody* a1 = static_cast<RigidBody*>(pairHeader.actors[0]->userData);
	RigidBody* a2 = static_cast<RigidBody*>(pairHeader.actors[1]->userData);

	for(auto callback : m_Callbacks) {
		callback(Ref<RigidBody>(a1), Ref<RigidBody>(a2));
	}
}
#endif

}