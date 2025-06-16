#include "World.h"

#include "Physics.h"

namespace Magma::Physics {

const float World::StepSize = 1.0f / 60.0f;

#ifdef MAGMA_PHYSICS
PxFilterFlags FilterShader(
	PxFilterObjectAttributes attributes0, PxFilterData filterData0,
	PxFilterObjectAttributes attributes1, PxFilterData filterData1,
	PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
	// All initial and persisting reports for everything, with per-point data
	pairFlags = PxPairFlag::eSOLVE_CONTACT
		| PxPairFlag::eDETECT_DISCRETE_CONTACT
		| PxPairFlag::eNOTIFY_TOUCH_FOUND
		// | PxPairFlag::eNOTIFY_TOUCH_PERSISTS
		// | PxPairFlag::eNOTIFY_CONTACT_POINTS
		;
	return PxFilterFlag::eDEFAULT;
}
#endif

World::World() {
#ifdef MAGMA_PHYSICS
	PxSceneDesc sceneDesc(GetPhysicsLib()->getTolerancesScale());
	sceneDesc.cpuDispatcher	= GetDispatcher();
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	sceneDesc.filterShader = FilterShader;
	sceneDesc.kineKineFilteringMode = physx::PxPairFilteringMode::eKEEP;
	sceneDesc.staticKineFilteringMode = physx::PxPairFilteringMode::eKEEP;
	sceneDesc.simulationEventCallback = &m_ContactCallback;
	m_Scene = GetPhysicsLib()->createScene(sceneDesc);

	m_Scene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f);
	m_Scene->setVisualizationParameter(PxVisualizationParameter::eACTOR_AXES, 1.0f);
	// m_Scene->setVisualizationParameter(PxVisualizationParameter::eBODY_AXES, 1.0f);
	m_Scene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);
	// m_Scene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_AXES, 1.0f);
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
	const Func<void, RigidBody*, RigidBody*>& callback)
{
#ifdef MAGMA_PHYSICS
	m_ContactCallback.AddCallback(callback);
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
		callback(a1, a2);
	}
}
#endif

}