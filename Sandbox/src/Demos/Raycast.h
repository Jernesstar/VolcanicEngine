#pragma once

#include <PxPhysics.h>
#include <PxPhysicsAPI.h>

using namespace physx;

static PxRigidStatic* gActor = nullptr;

static void createActor(const PxTransform& t, PxReal radius) {
	PxShape* shape = gPhysics->createShape(
		PxBoxGeometry(radius, radius, radius), *gMaterial);
	gActor = gPhysics->createRigidStatic(t);
	gActor->attachShape(*shape);
	gScene->addActor(*gActor);
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
	[&](const MouseButtonPressedEvent& event) {
		glm::vec4 originNDC{
			(event.x/800.0f - 0.5f) * 2.0f,
			(event.y/600.0f - 0.5f) * 2.0f,
			-1.0f, 1.0f
		};
		glm::vec4 endNDC{
			(event.x/800.0f - 0.5f) * 2.0f,
			(event.y/600.0f - 0.5f) * 2.0f,
			0.0f, 1.0f
		};

		glm::mat4 invViewProj = glm::inverse(camera->GetViewProjection());
		glm::vec4 worldStart = invViewProj * originNDC;
		glm::vec4 worldEnd   = invViewProj * endNDC;
		worldStart /= worldStart.w;
		worldEnd   /= worldEnd.w;
		glm::vec3 rayDir = glm::normalize(glm::vec3(worldEnd - worldStart));

		PxReal maxDist = 10000;
		PxHitFlags flags = PxHitFlag::ePOSITION
						 | PxHitFlag::eNORMAL
						 | PxHitFlag::eUV;
		// PxReal maxDist = 10.0e+5;
		PxRaycastBuffer hitInfo;
		PxVec3 start{ worldStart.x, worldStart.y, worldStart.z };
		PxVec3 dir{ rayDir.x, rayDir.y, rayDir.z };

		bool hit = gScene->raycast(start, dir, maxDist, hitInfo, flags);
		if(hit) {
			glm::vec3 n{ hitInfo.block.normal.x, hitInfo.block.normal.y, hitInfo.block.normal.z };
			VOLCANICORE_LOG_INFO("{ %f, %f, %f }", n.x, n.y, n.z);
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

	createActor(PxTransform(PxVec3(0.0, 0.0, 0.0)), 0.55);

	gScene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f);
	gScene->setVisualizationParameter(PxVisualizationParameter::eACTOR_AXES, 1.0f);
}

Raycast::~Raycast() {
	cleanupPhysics();
}

void Raycast::OnUpdate(TimeStep ts) {
	stepPhysics(ts);
	controller->OnUpdate(ts);
	shader->SetMat4("u_ViewProj", camera->GetViewProjection());

	Renderer::Clear();

	auto pose = gActor->getGlobalPose();

	Transform t{
		.Translation = { pose.p.x, pose.p.y, pose.p.z },
		.Rotation	 = { pose.q.x, pose.q.y, pose.q.z }
	};
	shader->SetMat4("u_Model", t.GetTransform());
	Renderer3D::DrawMesh(cube);

	const PxRenderBuffer& rb = scene->getRenderBuffer();

	for(PxU32 i=0; i < rb.getNbPoints(); i++)
	{
		const PxDebugPoint& point = rb.getPoints()[i];
		// render the point
	}

	for(PxU32 i=0; i < rb.getNbLines(); i++)
	{
		const PxDebugLine& line = rb.getLines()[i];
		// render the line
	}
}

}