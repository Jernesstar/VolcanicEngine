#pragma once

#include <PxPhysics.h>
#include <PxPhysicsAPI.h>

using namespace physx;

namespace Magma::Physics {

void Init();
void Close();

extern PxPhysics* GetPhysicsLib();
extern PxDefaultCpuDispatcher* GetDispatcher();

}