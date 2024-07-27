#include "Physics.h"

using namespace physx;

static PxDefaultAllocator	   s_Allocator;
static PxDefaultErrorCallback  s_ErrorCallback;
static PxFoundation*		   s_Foundation = nullptr;
static PxDefaultCpuDispatcher* s_Dispatcher = nullptr;

namespace Magma::Physics {

void PhysicsSystem::Init() {
	s_Foundation = PxCreateFoundation(PX_PHYSICS_VERSION, s_Allocator,
									  s_ErrorCallback);
	s_Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *s_Foundation,
								PxTolerancesScale(), true, nullptr);
	s_Dispatcher = PxDefaultCpuDispatcherCreate(2);
}

void PhysicsSystem::Close() {
	PX_RELEASE(s_Dispatcher);
	PX_RELEASE(s_Physics);
	PX_RELEASE(s_Foundation);
}

}