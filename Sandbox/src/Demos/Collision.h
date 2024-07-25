#pragma once

static void createWall(const PxTransform& t, PxU32 size, PxReal halfExtent)
{
	PxShape* shape = gPhysics->createShape(
		PxBoxGeometry(halfExtent, halfExtent, halfExtent), *gMaterial);
	for(PxU32 i = 0; i < size; i++)
	{
		for(PxU32 j = 0; j < size; j++)
		{
			PxTransform localTm(PxVec3(PxReal(j*2) - PxReal(size-i), PxReal(i*2+1), 0) * halfExtent);
			PxRigidStatic* body = gPhysics->createRigidStatic(t.transform(localTm));
			body->attachShape(*shape);
			gScene->addActor(*body);
		}
	}
	shape->release();
}

class ContactCallback : public PxSimulationEventCallback {
	void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count)	{ }
	void onWake(PxActor** actors, PxU32 count)							{ }
	void onSleep(PxActor** actors, PxU32 count)							{ }
	void onAdvance(const PxRigidBody* const *, const PxTransform*, const PxU32) { }
	void onTrigger(PxTriggerPair* pairs, PxU32 count);
	void onTrigger(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs);
}

void ContactCallback::onContact(const PxContactPairHeader& pairHeader,
								const PxContactPair* pairs, PxU32 nbPairs)
{
	// Retrieve the current poses and velocities of the two actors involved in the contact event.
	const PxTransform pose1 = pairHeader.actors[0]->getGlobalPose();
	const PxTransform pose2 = pairHeader.actors[1]->getGlobalPose();
}

bool gCheckpointReached;

void ContactCallback::onTrigger(PxTriggerPair* pairs, PxU32 count)
{
	for(PxU32 i=0; i<count; i++)
	{
		// ignore pairs when shapes have been deleted
		if(pairs[i].flags & (PxTriggerPairFlag::eREMOVED_SHAPE_TRIGGER | PxTriggerPairFlag::eREMOVED_SHAPE_OTHER))
			continue;

		// Detect for example that a player entered a checkpoint zone
		if((&pairs[i].otherShape->getActor() == gPlayerActor) &&
			(&pairs[i].triggerShape->getActor() == gSensorActor))
		{
			gCheckpointReached = true;
		}
	}
}

PxFilterFlags FilterShaderExample(
	PxFilterObjectAttributes attributes0, PxFilterData filterData0,
	PxFilterObjectAttributes attributes1, PxFilterData filterData1,
	PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
	VOLCANICORE_LOG_INFO("It's working");

	// let triggers through
	if(PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
	{
		pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
		return PxFilterFlag::eDEFAULT;
	}
	// generate contacts for all that were not filtered above
	pairFlags = PxPairFlag::eCONTACT_DEFAULT;

	// trigger the contact callback for pairs (A,B) where
	// the filtermask of A contains the ID of B and vice versa.
	if((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))
		pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;

	return PxFilterFlag::eDEFAULT;
}

ContactCallback gCallback;

namespace Demo {

class Collision : public Application {
public:
	Collision();
	~Collision();

	void OnUpdate(TimeStep ts);

private:
	Ref<ShaderPipeline> shader;
	Ref<Mesh> cube;

	Ref<Camera> camera;
	Ref<CameraController> controller;
};

Collision::Collision() {
	EventSystem::RegisterListener<KeyPressedEvent>(
	[](const KeyPressedEvent& event) {
		if(event.Key == Key::Escape)
			Application::Close();
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

	camera = CreateRef<StereographicCamera>(75.0f);
	camera->SetPosition({ 0.0f, 2.0f, -1.5f });
	controller = CreateRef<CameraController>(camera);

	initPhysics();

	createWall(PxTransform(PxVec3(0, 0, 0)), 10, 0.5f);

	PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	sceneDesc.cpuDispatcher	= gDispatcher;
	sceneDesc.filterShader	= FilterShaderExample;
	sceneDesc.simulationEventCallback = &gCallback;
	gScene = gPhysics->createScene(sceneDesc);

	PxRigidStatic* groundPlane = PxCreatePlane(*gPhysics, PxPlane(0,1,0,0), *gMaterial);
	gScene->addActor(*groundPlane);
}

Collision::~Collision() {
	cleanupPhysics();
}

void Collision::OnUpdate(TimeStep ts) {
	stepPhysics(ts);
}

}
