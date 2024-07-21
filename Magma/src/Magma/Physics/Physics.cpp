#include "Physics.h"

#include <PxPhysics.h>
#include <PxPhysicsAPI.h>

using namespace physx;

static PxDefaultAllocator		gAllocator;
static PxDefaultErrorCallback	gErrorCallback;
static PxFoundation*			gFoundation = nullptr;
static PxPhysics*				gPhysics	= nullptr;
static PxDefaultCpuDispatcher*	gDispatcher = nullptr;

namespace Magma::Physics {

void Init() {
	gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator,
									 gErrorCallback);
	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation,
							   PxTolerancesScale(), true, nullptr);
	gDispatcher = PxDefaultCpuDispatcherCreate(2);
}

void Close() {
	PX_RELEASE(gDispatcher);
	PX_RELEASE(gPhysics);
	PX_RELEASE(gFoundation);
}

}