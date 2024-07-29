#pragma once

#include <PxPhysics.h>
#include <PxPhysicsAPI.h>

using namespace physx;

static void createActors() {
	PxShape* shape = gPhysics->createShape(PxBoxGeometry(0.5, 0.5, 0.5), *gMaterial);
	for(uint32_t x = 0; x < 10; x++) {
		auto actor = gPhysics->createRigidStatic(PxTransform(PxVec3(x * 2.0f, 0.0f, 0.0f)));
		actor->attachShape(*shape);
		actor->userData = (void*)(uint64_t)x;
		gScene->addActor(*actor);
	}
	shape->release();
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

	EventSystem::RegisterListener<MouseButtonPressedEvent>(
	[this](const MouseButtonPressedEvent& event) {
		glm::vec4 originNDC{
			(event.x/800.0f - 0.5f) * 2.0f,
			(event.y/600.0f - 0.5f) * 2.0f,
			-1.0f, 1.0f
		};
		glm::vec4 endNDC{
			(event.x/800.0f - 0.5f) * 2.0f,
			(event.y/600.0f - 0.5f) * 2.0f,
			1.0f, 1.0f
		};

		glm::mat4 invViewProj = glm::inverse(camera->GetViewProjection());
		glm::vec4 worldStart = invViewProj * originNDC;
		glm::vec4 worldEnd   = invViewProj * endNDC;
		worldStart /= worldStart.w;
		worldEnd   /= worldEnd.w;
		glm::vec3 rayDir = glm::normalize(glm::vec3(worldEnd - worldStart));

		PxReal maxDist = 100000;
		PxHitFlags flags = PxHitFlag::ePOSITION
						 | PxHitFlag::eNORMAL
						 | PxHitFlag::eUV;
		// PxReal maxDist = 10.0e+5;
		PxRaycastBuffer hitInfo;
		PxVec3 start{ worldStart.x, worldStart.y, worldStart.z };
		PxVec3 dir{ rayDir.x, rayDir.y, rayDir.z };

		bool hit = gScene->raycast(start, dir, maxDist, hitInfo, flags);
		if(hit) {
			VOLCANICORE_LOG_INFO("{");
			VOLCANICORE_LOG_INFO("\tIndex: %i", (uint32_t)(uint64_t)hitInfo.block.actor->userData);
			VOLCANICORE_LOG_INFO("\tDistance: %i", (int)hitInfo.block.distance);
			VOLCANICORE_LOG_INFO("}\n");
		}
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

	camera = CreateRef<StereographicCamera>(75.0f);
	camera->SetPosition({ 0.0f, 0.0f, 3.0f });
	controller = CreateRef<CameraController>(camera);

	initPhysics();

	PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -90.81f, 0.0f);
	sceneDesc.cpuDispatcher	= gDispatcher;
	sceneDesc.filterShader	= PxDefaultSimulationFilterShader;
	// sceneDesc.flags = PxSceneFlag::eENABLE_ACTIVE_ACTORS;
	gScene = gPhysics->createScene(sceneDesc);

	createActors();

	// gScene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f);
	// gScene->setVisualizationParameter(PxVisualizationParameter::eACTOR_AXES, 1.0f);
}

Raycast::~Raycast() {
	cleanupPhysics();
}

void Raycast::OnUpdate(TimeStep ts) {
	stepPhysics(ts);
	controller->OnUpdate(ts);
	shader->SetMat4("u_ViewProj", camera->GetViewProjection());

	Renderer::Clear();

	PxU32 nbActors;
	PxActor** actors = (PxActor**)malloc(20 * sizeof(PxActor*));
	nbActors = gScene->getActors(PxActorTypeFlag::eRIGID_STATIC, actors, 10);

	// update each render object with the new transform
	for(PxU32 i = 0; i < nbActors; i++) {
		// RigidBody* body = static_cast<RigidBody*>(actors[i]->userData);

		PxRigidActor& rd = static_cast<PxRigidActor&>(*actors[i]);
		auto pose = rd.getGlobalPose();

		Transform t{
			.Translation = { pose.p.x, pose.p.y, pose.p.z },
			// .Rotation	 = { pose.q.x, pose.q.y, pose.q.z }
		};
		shader->SetMat4("u_Model", t.GetTransform());
		Renderer3D::DrawMesh(cube);
	}

	// const PxRenderBuffer& rb = gScene->getRenderBuffer();

	// for(PxU32 i=0; i < rb.getNbPoints(); i++)
	// {
	// 	const PxDebugPoint& point = rb.getPoints()[i];
		
	// }

	// for(PxU32 i=0; i < rb.getNbLines(); i++)
	// {
	// 	const PxDebugLine& line = rb.getLines()[i];
		
	// }
}

}