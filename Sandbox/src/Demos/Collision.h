#pragma once

static void createWall(const PxTransform& t, PxU32 size, PxReal halfExtent)
{
	PxShape* shape = gPhysics->createShape(PxBoxGeometry(halfExtent, halfExtent,
														 halfExtent),
														 *gMaterial);
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

	int n = 5;
	for(PxU32 i = 0; i < n; i++)
		createWall(PxTransform(PxVec3(0, 0, stackZ -= 10.0f)), 10, 0.5f);

	// RigidBody player;
	// RididBody wall;

	// player.CollidesWith(wall, CollisionBehaviour::Stop); // The wall is inmovable
}

Collision::~Collision() {
	cleanupPhysics();
}

void Collision::OnUpdate(TimeStep ts) {
	stepPhysics(ts);

	Renderer::Clear();
	
}

}