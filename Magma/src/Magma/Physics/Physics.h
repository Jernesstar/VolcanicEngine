#pragma once

#include <PxPhysics.h>
#include <PxPhysicsAPI.h>

using namespace physx;

namespace Magma::Physics {

class PhysicsSystem {
public:
	static void Init();
	static void Close();

private:
	inline static PxPhysics* s_Physics				   = nullptr;
	inline static PxDefaultCpuDispatcher* s_Dispatcher = nullptr;

	friend class World;
	friend class RigidBody;
};

}