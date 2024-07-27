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
	RigidBody(RigidBodyType type);
	// RigidBody(RigidBodyType type, const Shape& shape);
	~RigidBody();

protected:
	PxRigidActor* m_Actor;
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

private:
	
};

}