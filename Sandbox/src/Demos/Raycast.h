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
static PxPvd*					gPvd		= nullptr;
static PxRigidStatic*			gActor		= nullptr;

static void createActor(const PxTransform& t, PxU32 size, PxReal halfExtent) {
	PxShape* shape = gPhysics->createShape(
		PxBoxGeometry(halfExtent, halfExtent, halfExtent), *gMaterial);
	PxTransform localTm(PxVec3(0.0, 0.0, 0.0));
	gActor = gPhysics->createRigidStatic(t.transform(localTm));
	gActor->attachShape(*shape);
	gScene->addActor(*body);
	shape->release();
}

void initPhysics() {
	gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator,
									 gErrorCallback);
	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation,
							   PxTolerancesScale(), true, gPvd);
	gDispatcher = PxDefaultCpuDispatcherCreate(2);

	PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -90.81f, 0.0f);
	sceneDesc.cpuDispatcher	= gDispatcher;
	sceneDesc.filterShader	= PxDefaultSimulationFilterShader;
	sceneDesc.flags = PxSceneFlag::eENABLE_ACTIVE_ACTORS;
	gScene = gPhysics->createScene(sceneDesc);

	gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);

	PxRigidStatic* groundPlane = PxCreatePlane(*gPhysics,
											   PxPlane(0,1,0,0), *gMaterial);
	gScene->addActor(*groundPlane);

	createActor(PxTransform(PxVec3(0.0, 0.0, 0.0)), 10, 2.5);
}

void stepPhysics(TimeStep ts) {
	gScene->simulate(1.0f/60.0f);
	// gScene->simulate((float)ts);
	gScene->fetchResults(true);
}
	
void cleanupPhysics() {
	PX_RELEASE(gScene);
	PX_RELEASE(gDispatcher);
	PX_RELEASE(gPhysics);
	PX_RELEASE(gFoundation);
}

namespace Demo {

class Raycast : public Application {
public:
	Raycast();
	~Raycast();

	void OnUpdate(TimeStep ts);

private:
	Ref<ShaderPipeline> shader;
	Ref<Mesh> cube;

	Ref<Camera> camera;
	Ref<CameraController> controller;
};

Raycast::Raycast() {
	EventSystem::RegisterListener<KeyPressedEvent>(
	[](const KeyPressedEvent& event) {
		if(event.Key == Key::Escape)
			Application::Close();
	});

	EventSystem::RegisterListener<MouseButtonPressed>(
	[&camera](const MouseButtonPressedEvent& event) {
		glm::vec4 origin{
			(event.x/600.0f - 0.5f) * 2.0f,
			(event.y/800.0f - 0.5f) * 2.0f,
			-1.0f, 1.0f
		};
		glm::vec4 end{
			(event.x/600.0f - 0.5f) * 2.0f,
			(event.y/800.0f - 0.5f) * 2.0f,
			0.0f, 1.0f
		};

		glm::mat4 invViewProj = glm::inverse(camera->GetViewProjection());
		glm::vec4 worldStart = invViewProj * origin;
		glm::vec4 worldEnd   = invViewProj * end;
		worldStart /= worldStart.w;
		worldEnd  /= worldEnd.w;
		glm::vec3 rayDir = glm::normalize(glm::vec3(worldEnd - worldStart));

		PxReal maxDist = 1000;
		PxHitFlags flags = PxHitFlag::ePOSITION
									| PxHitFlag::eNORMAL
									| PxHitFlag::eUV;
		// PxReal maxDist = 10.0e+5;
		PxRaycastBuffer hitInfo;

		bool hit = gScene->raycast(worldStart, rayDir, maxDist, hitInfo, flags);
		if(hit)
			VOLCANICORE_LOG_INFO("Hit");
		else
			VOLCANICORE_LOG_INFO("No hit");
	});

	cube = Mesh::Create(MeshPrimitive::Cube,
	Material{
		.Diffuse = Texture::Create("Sandbox/assets/images/wood.png")
		// .Specular = Texture::Create("Sandbox/assets/images/wood_specular.png"),
	});
	shader = ShaderPipeline::Create({
		{ "VolcaniCore/assets/shaders/Mesh.glsl.vert", ShaderType::Vertex },
		{ "VolcaniCore/assets/shaders/Mesh.glsl.frag", ShaderType::Fragment }
	});
	shader->Bind();
	shader->SetTexture("u_Diffuse", cube->GetMaterial().Diffuse, 0);

	camera = CreateRef<StereographicCamera>(75.0f, 0.01f, 1000.0f, 800, 600);
	camera->SetPosition({ 0.0f, 2.0f, -1.5f });
	controller = CreateRef<CameraController>(camera);

	initPhysics();
}

Raycast::~Raycast() {
	cleanupPhysics();
}

void Raycast::OnUpdate(TimeStep ts) {
	stepPhysics(ts);
	controller->OnUpdate(ts);
	shader->SetMat4("u_ViewProj", camera->GetViewProjection());

	auto pose = gActor->getGlobalPose();

	Transform t{
		.Translation = { pose.p.x, pose.p.y, pose.p.z },
		.Rotation	 = { pose.q.x, pose.q.y, pose.q.z }
	};
	shader->SetMat4("u_Model", t.GetTransform());
	Renderer3D::DrawMesh(cube);
}

}