#pragma once

#ifdef MAGMA_PHYSICS
#include <PxPhysics.h>
#include <PxPhysicsAPI.h>

using namespace physx;
#endif

#include <VolcaniCore/Core/List.h>
#include <VolcaniCore/Core/Time.h>

using namespace VolcaniCore;

#include "RigidBody.h"

namespace Magma::Physics {

#ifdef MAGMA_PHYSICS
class ContactCallback : public PxSimulationEventCallback {
public:
	void AddCallback(
		const Func<void, RigidBody*, RigidBody*>& callback)
	{
		m_Callbacks.Add(callback);
	}

private:
	void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) { }
	void onWake(PxActor** actors, PxU32 count) { }
	void onSleep(PxActor** actors, PxU32 count) { }
	void onAdvance(const PxRigidBody*const*, const PxTransform*, const PxU32) {}
	void onTrigger(PxTriggerPair* pairs, PxU32 count);
	void onContact(const PxContactPairHeader& pairHeader,
				   const PxContactPair* pairs, PxU32 nbPairs);

private:
	List<Func<void, RigidBody*, RigidBody*>> m_Callbacks;
};

#endif

struct HitInfo {
	const bool HasHit;
	RigidBody* Actor;
	const float Distance;

	HitInfo()
		: HasHit(false), Actor(nullptr), Distance(0.0f) { }
	HitInfo(RigidBody* actor, float distance)
		: HasHit(true), Actor(actor), Distance(distance) { }
	
	operator bool() const { return HasHit; }
};

class World {
public:
	static const float StepSize;

public:
	World();
	~World();

	void OnUpdate(TimeStep ts);

	void AddActor(Ref<RigidBody> body);
	// RigidBody CreateActor();
	// RigidBody CreatePlane();

	HitInfo Raycast(const glm::vec3& start,
					const glm::vec3& direction, float maxDist = 10000.0f);

	void AddContactCallback(
		const Func<void, RigidBody*, RigidBody*>& callback);

	const List<Ref<RigidBody>>& GetActors() const { return m_Actors; }

	List<Ref<RigidBody>>::const_iterator begin() { return m_Actors.begin(); }
	List<Ref<RigidBody>>::const_iterator end() { return m_Actors.end(); }

#ifdef MAGMA_PHYSICS
	PxScene* Get() { return m_Scene; }
#endif

private:

#ifdef MAGMA_PHYSICS
	PxScene* m_Scene = nullptr;
	ContactCallback m_ContactCallback;
#endif

	uint64_t m_ActorCount = 0;
	uint64_t m_MaxActorCount = 0;
	List<Ref<RigidBody>> m_Actors;

	float m_Accumulator = 0.0f;
};

}