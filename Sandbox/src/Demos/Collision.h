#pragma once

static void createWall(Physics::World& world) {
	Ref<Shape> box = Shape::Create(Shape::Type::Box);
	for(uint32_t i = 0; i < 4; i++) {
		for(uint32_t j = 0; j < 4; j++) {
			Ref<RigidBody> body =
				RigidBody::Create(RigidBody::Type::Dynamic, box,
					Transform
					{
						.Translation = { j*2 - (4 - i), i*2 + 1, 0.0f },
					});

			world.AddActor(body);
		}
	}
}

// PxFilterFlags FilterShaderExample(
// 	PxFilterObjectAttributes attributes0, PxFilterData filterData0,
// 	PxFilterObjectAttributes attributes1, PxFilterData filterData1,
// 	PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
// {
// 	// // let triggers through
// 	// if(PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
// 	// {
// 	// 	pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
// 	// 	return PxFilterFlag::eDEFAULT;
// 	// }
// 	// // generate contacts for all that were not filtered above
// 	// pairFlags = PxPairFlag::eCONTACT_DEFAULT;

// 	// // trigger the contact callback for pairs (A,B) where
// 	// // the filtermask of A contains the ID of B and vice versa.
// 	// if((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))
// 	// 	pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;

// 	// return PxFilterFlag::eDEFAULT;

// 	// all initial and persisting reports for everything, with per-point data
// 	pairFlags = PxPairFlag::eSOLVE_CONTACT
// 				| PxPairFlag::eDETECT_DISCRETE_CONTACT
// 				| PxPairFlag::eNOTIFY_TOUCH_FOUND
// 				| PxPairFlag::eNOTIFY_TOUCH_PERSISTS
// 				| PxPairFlag::eNOTIFY_CONTACT_POINTS;

// 	return PxFilterFlag::eDEFAULT;
// }

namespace Demo {

class Collision : public Application {
public:
	Collision();
	~Collision();

	void OnUpdate(TimeStep ts);

private:
	Ref<Physics::World> world;

	Ref<RenderPass> drawPass;
	Ref<Mesh> cube;

	Ref<Camera> camera;
	CameraController controller;
};

Collision::Collision() {
	Events::RegisterListener<KeyPressedEvent>(
		[](const KeyPressedEvent& event)
		{
			if(event.Key == Key::Escape)
				Application::Close();
		});

	Ref<ShaderPipeline> shader;
	shader = ShaderPipeline::Create("VolcaniCore/assets/shaders", "Mesh");
	drawPass = RenderPass::Create("Draw", shader);
	drawPass->SetData(Renderer3D::GetMeshBuffer());

	cube = Mesh::Create(MeshPrimitive::Cube,
		Material
		{
			.Diffuse = Texture::Create("Sandbox/assets/images/wood.png")
		});

	Physics::Init();

	world = CreateRef<Physics::World>();
	createWall(*world);

	// auto plane = RigidBody::Create(RigidBody::Type::Static, Shape(Shape::Type::Plane));
	// world.AddActor(plane);

	camera = CreateRef<StereographicCamera>(75.0f);
	camera->SetPosition({ 0.0f, 0.5f, 3.0f });
	controller = CameraController{ camera };
}

Collision::~Collision() {
	Physics::Close();
}

void Collision::OnUpdate(TimeStep ts) {
	controller.OnUpdate(ts);
	world->OnUpdate(ts);

	Renderer::StartPass(drawPass);
	{
		Renderer::Clear();

		Renderer3D::Begin(camera);

		for(auto body : *world) {
			body->UpdateTransform();
			Renderer3D::DrawMesh(cube, body->GetTransform());
		}

		Renderer3D::End();
	}
	Renderer::EndPass();
}

}
