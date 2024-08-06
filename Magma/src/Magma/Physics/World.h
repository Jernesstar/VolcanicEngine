#pragma once

#include <PxPhysics.h>
#include <PxPhysicsAPI.h>

using namespace physx;

#include <VolcaniCore/Core/Time.h>

using namespace VolcaniCore;

#include "RigidBody.h"

namespace Magma::Physics {

// TODO: This may cause compilation errors
class ContactCallback;
class HitInfo;

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

	void AddContactCallback(RigidBody& actor1, RigidBody& actor2,
		const std::function<void(RigidBody&, RigidBody&)>& callback);

	const std::vector<Ref<RigidBody>>& GetActors() const { return m_Actors; }

	std::vector<Ref<RigidBody>>::const_iterator begin() {
		return m_Actors.begin();
	}
	std::vector<Ref<RigidBody>>::const_iterator end() {
		return m_Actors.end();
	}

	HitInfo Raycast(const glm::vec3& start,
					const glm::vec3& direction, float maxDist = 10000.0f);

	PxScene* Get() { return m_Scene; }

private:
	PxScene* m_Scene = nullptr;
	ContactCallback m_ContactCallback;

	uint64_t m_ActorCount = 0;
	uint64_t m_MaxActorCount = 0;
	std::vector<Ref<RigidBody>> m_Actors;

	float m_Accumulator = 0.0f;
};

class ContactCallback : public PxSimulationEventCallback {
public:
	void AddCallback(
		const std::function<void(RigidBody&, RigidBody&)>& callback)
	{
		m_Callbacks.push_back(callback);
	}

private:
	void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) { }
	void onWake(PxActor** actors, PxU32 count) { }
	void onSleep(PxActor** actors, PxU32 count) { }
	void onAdvance(const PxRigidBody* const*, const PxTransform*, const PxU32)
	{
	}
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
	HitInfo()
		: HasHit(false), Distance(0.0f), m_Actor(nullptr) { }
	HitInfo(RigidBody* actor, float distance)
		: HasHit(true), Distance(distance), m_Actor(actor) { }

	// Call only if HasHit is true
	RigidBody& GetActor() {
		return *m_Actor;
	}

private:
	RigidBody* m_Actor;
};

}