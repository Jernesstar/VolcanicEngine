#pragma once

#include <PxPhysics.h>
#include <PxPhysicsAPI.h>

using namespace physx;

static PxDefaultAllocator		gAllocator;
static PxDefaultErrorCallback	gErrorCallback;
static PxFoundation*			gFoundation = nullptr;
static PxPhysics*				gPhysics	= nullptr;
static PxDefaultCpuDispatcher*	gDispatcher = nullptr;
static PxScene*					gScene		= nullptr;
static PxMaterial*				gMaterial	= nullptr;
static PxPvd*					gPvd        = nullptr;

static PxReal stackZ = 10.0f;

static PxRigidDynamic* createDynamic(const PxTransform& t,
									 const PxGeometry& geometry,
									 const PxVec3& velocity=PxVec3(0))
{
	PxRigidDynamic* dynamic = PxCreateDynamic(*gPhysics, t, geometry,
											  *gMaterial, 10.0f);
	dynamic->setAngularDamping(0.5f);
	dynamic->setLinearVelocity(velocity);
	gScene->addActor(*dynamic);
	return dynamic;
}

static void createStack(const PxTransform& t, PxU32 size, PxReal halfExtent)
{
	PxShape* shape = gPhysics->createShape(PxBoxGeometry(halfExtent, halfExtent,
														 halfExtent),
														 *gMaterial);
	for(PxU32 i = 0; i < size; i++)
	{
		for(PxU32 j = 0; j < size - i; j++)
		{
			PxTransform localTm(PxVec3(PxReal(j*2) - PxReal(size-i), PxReal(i*2+1), 0) * halfExtent);
			PxRigidDynamic* body = gPhysics->createRigidDynamic(t.transform(localTm));
			body->attachShape(*shape);
			PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
			gScene->addActor(*body);
		}
	}
	shape->release();
}

void initPhysics()
{
	gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator,
									 gErrorCallback);
	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation,
							   PxTolerancesScale(), true, gPvd);
	gDispatcher = PxDefaultCpuDispatcherCreate(2);

	gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);
}

void stepPhysics(TimeStep ts)
{
	gScene->simulate(1.0f/60.0f);
	// gScene->simulate((float)ts);
	gScene->fetchResults(true);
}
	
void cleanupPhysics()
{
	PX_RELEASE(gScene);
	PX_RELEASE(gDispatcher);
	PX_RELEASE(gPhysics);
	PX_RELEASE(gFoundation);
}
