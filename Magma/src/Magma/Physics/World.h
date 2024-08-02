#pragma once

#include <PxPhysics.h>
#include <PxPhysicsAPI.h>

using namespace physx;

#include <VolcaniCore/Core/Time.h>

using namespace VolcaniCore;

#include "RigidBody.h"

namespace Magma::Physics {

class ContactCallback : public PxSimulationEventCallback {
private:
	void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) { }
	void onWake(PxActor** actors, PxU32 count) { }
	void onSleep(PxActor** actors, PxU32 count) { }
	void onAdvance(const PxRigidBody* const *, const PxTransform*, const PxU32) { }
	void onTrigger(PxTriggerPair* pairs, PxU32 count);
	void onContact(const PxContactPairHeader& pairHeader,
				   const PxContactPair* pairs, PxU32 nbPairs);

private:
	std::vector<std::function<void(RigidBody&, RigidBody&)>> m_Callbacks;
};

class HitInfo {
public:
	const bool HasHit;
	const float Distance;

public:
	HitInfo(RigidBody* actor, float distance)
		: m_Actor(actor), HasHit(true), Distance(distance) { }
	HitInfo()
		: m_Actor(nullptr), HasHit(false), Distance(0.0f) { }

	// Call only if HasHit is true
	RigidBody& GetActor() {
		return *m_Actor;
	}

private:
	RigidBody* m_Actor;
};

class World {
public:
	static constexpr float StepSize = 1.0f / 60.0f;

public:
	World();
	~World();

	void OnUpdate(TimeStep ts);

	void AddActor(Ref<RigidBody> body);
	// RigidBody CreateActor();
	// RigidBody CreatePlane();

	void AddContactCallback(
		const std::function<void(RigidBody&, RigidBody&)>& callback);

	const std::vector<Ref<RigidBody>>& GetActors() const { return m_Actors; }

	std::vector<Ref<RigidBody>>::const_iterator begin() {
		return m_Actors.begin();
	}
	std::vector<Ref<RigidBody>>::const_iterator end() {
		return m_Actors.end();
	}
	// Sends a ray through the scene to see if it hits something
	HitInfo Raycast(const glm::vec3& start,
					const glm::vec3& direction, float maxDist = 10000.0f);

private:
	ContactCallback m_ContactCallback;

	PxScene* m_Scene = nullptr;
	uint64_t m_ActorCount = 0;
	uint64_t m_MaxActorCount = 0;
	std::vector<Ref<RigidBody>> m_Actors;

	float m_Accumulator = 0.0f;
};

}