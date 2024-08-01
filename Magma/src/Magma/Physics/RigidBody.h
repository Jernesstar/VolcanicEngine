#pragma once

#include <PxPhysics.h>
#include <PxPhysicsAPI.h>

using namespace physx;

namespace Magma::Physics {

enum class RigidBodyType { Static, Dynamic };

class RigidBody {
public:
	const RigidBodyType Type;

public:
	RigidBody(RigidBodyType type, const Shape& shape, const Transform& t = { });
	~RigidBody();

	void UpdateTransform() const;
	Transform GetTransform() const { return m_Transform; }

	template<typename TDerived>
	requires std::derived_from<TDerived, RigidBody>
	TDerived& As() const { return *(TDerived*)(this); }

protected:
	PxRigidActor* m_Actor;
	Transform m_Transform;

	friend class World;
};

class StaticBody : public RigidBody {
public:
	StaticBody();
	~StaticBody() = default;

private:
	
};

class DynamicBody : public RigidBody {
public:
	DynamicBody();
	~DynamicBody() = default;

	// void SetVelocity(const glm::vec3& velocity);

private:
	
};

}