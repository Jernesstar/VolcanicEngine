#pragma once

#include <PxPhysics.h>
#include <PxPhysicsAPI.h>

using namespace physx;
using namespace VolcaniCore;

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
							   PxTolerancesScale(), true, nullptr);

	PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	gDispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher	= gDispatcher;
	sceneDesc.filterShader	= PxDefaultSimulationFilterShader;
	gScene = gPhysics->createScene(sceneDesc);

	gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);

	PxRigidStatic* groundPlane = PxCreatePlane(*gPhysics, PxPlane(0,1,0,0), *gMaterial);
	gScene->addActor(*groundPlane);

	int n = 1;
	for(PxU32 i = 0; i < n; i++)
		createStack(PxTransform(PxVec3(0, 0, stackZ -= 10.0f)), 10, 2.0f);
}

void stepPhysics(TimeStep ts)
{
	gScene->simulate((float)ts);
	gScene->fetchResults(true);
}
	
void cleanupPhysics()
{
	PX_RELEASE(gScene);
	PX_RELEASE(gDispatcher);
	PX_RELEASE(gPhysics);
	if(gPvd) {
		PxPvdTransport* transport = gPvd->getTransport();
		PX_RELEASE(gPvd);
		PX_RELEASE(transport);
	}
	PX_RELEASE(gFoundation);
}

namespace Demo {

class Physics : public Application {
public:
	Physics();
	~Physics();

	void OnUpdate(TimeStep ts);

private:
	Ref<ShaderPipeline> shader;
	Ref<Mesh> cube;
};

Physics::Physics() {
	EventSystem::RegisterListener<KeyPressedEvent>(
	[](const KeyPressedEvent& event) {
		if(event.Key == Key::Escape)
			Application::Close();
	});

	initPhysics();
	// EventSystem::RegisterListener<MouseButtonPressed>(
	// [](const MouseButtonPressedEvent& event) {
	// 	PxRaycastHit hitInfo;
	// 	const PxU32 maxHits = 1;
	// 	const PxHitFlags hitFlags = PxHitFlag::ePOSITION
	// 								| PxHitFlag::eNORMAL
	// 								| PxHitFlag::eUV;
	// 	PxU32 hitCount = PxGeometryQuery::raycast(origin, unitDir, geom, pose,
	// 											  maxDist, hitFlags, maxHits,
	// 											  &hitInfo);
	// });

	cube = Mesh::Create(MeshPrimitive::Cube,
	Material{
		.Diffuse = Texture::Create("Sandbox/assets/images/wood.png"),
		.Specular = Texture::Create("Sandbox/assets/images/wood_specular.png"),
	});
	shader = ShaderPipeline::Create({
		{ "VolcaniCore/assets/shaders/Mesh.glsl.vert", ShaderType::Vertex },
		{ "VolcaniCore/assets/shaders/Mesh.glsl.frag", ShaderType::Fragment }
	});
	shader->Bind();
}

Physics::~Physics() {
	cleanupPhysics();
}

void Physics::OnUpdate(TimeStep ts) {
	stepPhysics(ts);

	// retrieve array of actors that moved
	PxU32 nbActiveActors;
	PxActor** activeActors = gScene->getActiveActors(nbActiveActors);

	Renderer::Clear();

	// update each render object with the new transform
	for(PxU32 i = 0; i < nbActiveActors; i++) {
		// MyRenderObject* renderObject = static_cast<MyRenderObject*>(activeActors[i]->userData);

		auto mat4 = activeActors[i]->getGlobalPose();
		shader->SetMat4("u_Model", mat4);
		Renderer3D::DrawMesh(cube);
	}
}

}