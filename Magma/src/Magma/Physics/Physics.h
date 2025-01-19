#pragma once

#include <VolcaniCore/Core/Defines.h>

#ifdef MAGMA_PHYSICS
#define PX_PHYSX_STATIC_LIB

#include <PxPhysics.h>
#include <PxPhysicsAPI.h>
using namespace physx;

#endif

namespace Magma::Physics {

extern void Init();
extern void Close();

#ifdef MAGMA_PHYSICS
extern PxPhysics* GetPhysicsLib();
extern PxDefaultCpuDispatcher* GetDispatcher();
#endif

}