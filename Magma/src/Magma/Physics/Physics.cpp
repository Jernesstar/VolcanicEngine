#include "Physics.h"

using namespace physx;

static PxDefaultAllocator	   gAllocator;
static PxDefaultErrorCallback  gErrorCallback;
static PxFoundation*		   gFoundation = nullptr;
static PxDefaultCpuDispatcher* gDispatcher = nullptr;

namespace Magma::Physics {

void PhysicsSystem::Init() {
	s_Foundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator,
									  gErrorCallback);
	s_Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation,
								PxTolerancesScale(), true, nullptr);
	s_Dispatcher = PxDefaultCpuDispatcherCreate(2);
}

void PhysicsSystem::Close() {
	PX_RELEASE(s_Dispatcher);
	PX_RELEASE(s_Physics);
	PX_RELEASE(s_Foundation);
}

}