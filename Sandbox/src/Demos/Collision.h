#pragma once

static void createWall(Physics::World& world) {
	Shape box(ShapeType::Box);
	for(uint32_t i = 0; i < 4; i++) {
		for(uint32_t j = 0; j < 4; j++) {
			RigidBody body(RigidBodyType::Dynamic, box,
				Transform{
					.Translation = 0.5f * { j*2 - (4 - i), i*2 + 1, 0.0f },
				});
			
			world.AddActor(body);
		}
	}
}

PxFilterFlags FilterShaderExample(
	PxFilterObjectAttributes attributes0, PxFilterData filterData0,
	PxFilterObjectAttributes attributes1, PxFilterData filterData1,
	PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
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

	Physics::World world;
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

	Physics::Init();

	createWall(world);

	world.AddActor(RigidBody(RigidBodyType::Static, Shape(ShapeType::Plane)));
}

Collision::~Collision() {
	Physics::Close();
}

void Collision::OnUpdate(TimeStep ts) {
	world.OnUpdate(ts);

	controller->OnUpdate(ts);
	shader->SetMat4("u_ViewProj", camera->GetViewProjection());

	Renderer::Clear();

	for(auto* body : world.GetActors()) {
		shader->SetMat4("u_Model", body->GetTransform());
		Renderer3D::DrawMesh(cube);
	}
}

}
