#include "Physics.h"

#ifdef MAGMA_PHYSICS
static PxDefaultAllocator	   s_Allocator;
static PxDefaultErrorCallback  s_ErrorCallback;
static PxFoundation*		   s_Foundation = nullptr;
static PxPhysics*			   s_Physics	= nullptr;
static PxDefaultCpuDispatcher* s_Dispatcher = nullptr;
#endif

namespace Magma::Physics {

void Init() {
#ifdef MAGMA_PHYSICS
	s_Foundation =
		PxCreateFoundation(PX_PHYSICS_VERSION, s_Allocator, s_ErrorCallback);
	s_Physics =
		PxCreatePhysics(PX_PHYSICS_VERSION, *s_Foundation, PxTolerancesScale());
	s_Dispatcher = PxDefaultCpuDispatcherCreate(2);
#endif
}

void Close() {
#ifdef MAGMA_PHYSICS
	// PX_RELEASE(s_Dispatcher);
	PX_RELEASE(s_Physics);
	PX_RELEASE(s_Foundation);
#endif
}

#ifdef MAGMA_PHYSICS
PxPhysics* GetPhysicsLib() { return s_Physics; }
PxDefaultCpuDispatcher* GetDispatcher() { return s_Dispatcher; }
#endif

}