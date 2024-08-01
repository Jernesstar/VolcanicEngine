#pragma once

#include <PxPhysics.h>
#include <PxPhysicsAPI.h>

using namespace physx;

namespace Magma::Physics {

static void Init();
static void Close();

inline static PxPhysics*			  s_Physics	   = nullptr;
inline static PxDefaultCpuDispatcher* s_Dispatcher = nullptr;

}