#pragma once

#define PX_PHYSX_STATIC_LIB

#include <PxPhysics.h>
#include <PxPhysicsAPI.h>

using namespace physx;

namespace Magma::Physics {

extern void Init();
extern void Close();

extern PxPhysics* GetPhysicsLib();
extern PxDefaultCpuDispatcher* GetDispatcher();

}