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
			PxRigidDynamic* body = gPhysics->createRigidDynamic(t.transform(localTm));
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
	void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs);
};

void ContactCallback::onTrigger(PxTriggerPair* pairs, PxU32 count)
{
	// for(PxU32 i=0; i<count; i++)
	// {
	// 	ignore pairs when shapes have been deleted
	// 	if(pairs[i].flags & (PxTriggerPairFlag::eREMOVED_SHAPE_TRIGGER | PxTriggerPairFlag::eREMOVED_SHAPE_OTHER))
	// 		continue;

	// 	Detect for example that a player entered a checkpoint zone
	// 	if((&pairs[i].otherShape->getActor() == gPlayerActor) &&
	// 		(&pairs[i].triggerShape->getActor() == gSensorActor))
	// 	{
	// 		gCheckpointReached = true;
	// 	}
	// }
}

void ContactCallback::onContact(const PxContactPairHeader& pairHeader,
								const PxContactPair* pairs, PxU32 nbPairs)
{
	// Retrieve the current poses and velocities of the two actors involved in the contact event.
	PxRigidActor& actor1 = static_cast<PxRigidActor&>(*pairHeader.actors[0]);
	PxRigidActor& actor2 = static_cast<PxRigidActor&>(*pairHeader.actors[1]);

	VOLCANICORE_LOG_INFO("It's working");
}

PxFilterFlags FilterShaderExample(
	PxFilterObjectAttributes attributes0, PxFilterData filterData0,
	PxFilterObjectAttributes attributes1, PxFilterData filterData1,
	PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
	VOLCANICORE_LOG_INFO("This is working instead");

	// // let triggers through
	// if(PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
	// {
	// 	pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
	// 	return PxFilterFlag::eDEFAULT;
	// }
	// // generate contacts for all that were not filtered above
	// pairFlags = PxPairFlag::eCONTACT_DEFAULT;

	// // trigger the contact callback for pairs (A,B) where
	// // the filtermask of A contains the ID of B and vice versa.
	// if((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))
	// 	pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;

	// return PxFilterFlag::eDEFAULT;

	// all initial and persisting reports for everything, with per-point data
	pairFlags = PxPairFlag::eSOLVE_CONTACT
				| PxPairFlag::eDETECT_DISCRETE_CONTACT
				| PxPairFlag::eNOTIFY_TOUCH_FOUND 
				| PxPairFlag::eNOTIFY_TOUCH_PERSISTS
				| PxPairFlag::eNOTIFY_CONTACT_POINTS;

	return PxFilterFlag::eDEFAULT;
}

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

	ContactCallback gCallback;
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
	camera->SetPosition({ 0.0f, 0.5f, 3.0f });
	controller = CreateRef<CameraController>(camera);

	initPhysics();

	PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	sceneDesc.cpuDispatcher	= gDispatcher;
	sceneDesc.filterShader	= FilterShaderExample;
	// sceneDesc.simulationEventCallback = new ContactCallback();
	sceneDesc.simulationEventCallback = &gCallback;
	gScene = gPhysics->createScene(sceneDesc);

	createWall(PxTransform(PxVec3(0, 0, 0)), 4, 0.5f);

	PxRigidStatic* groundPlane = PxCreatePlane(*gPhysics, PxPlane(0,1,0,0), *gMaterial);
	gScene->addActor(*groundPlane);
}

Collision::~Collision() {
	cleanupPhysics();
}

void Collision::OnUpdate(TimeStep ts) {
	stepPhysics(ts);

	controller->OnUpdate(ts);
	shader->SetMat4("u_ViewProj", camera->GetViewProjection());

	Renderer::Clear();

	PxU32 nbActors;
	PxActor** actors = (PxActor**)malloc(20 * sizeof(PxActor*));
	nbActors = gScene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC, actors, 20);

	// update each render object with the new transform
	for(PxU32 i = 0; i < nbActors; i++) {
		// RigidBody* body = static_cast<RigidBody*>(actors[i]->userData);

		if(actors[i]->getConcreteType() == PxConcreteType::eRIGID_DYNAMIC)
		{
			PxRigidDynamic& rd = static_cast<PxRigidDynamic&>(*actors[i]);
			auto pose = rd.getGlobalPose();

			Transform t{
				.Translation = { pose.p.x, pose.p.y, pose.p.z },
				// .Rotation	 = { pose.q.x, pose.q.y, pose.q.z }
			};
			shader->SetMat4("u_Model", t.GetTransform());
			Renderer3D::DrawMesh(cube);
		}
	}
}

}
