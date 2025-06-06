#pragma once

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
	shader = AssetImporter::GetShader(
		{
			{ "Magma/assets/shaders/Mesh.glsl.vert", ShaderType::Vertex },
			{ "Magma/assets/shaders/Mesh.glsl.frag", ShaderType::Fragment }
		});
	drawPass = RenderPass::Create("Draw", shader);
	drawPass->SetData(Renderer3D::GetMeshBuffer());

	cube = Mesh::Create(MeshType::Cube,
		{
			.Diffuse = AssetImporter::GetTexture("Sandbox/assets/images/wood.png")
		});

	Physics::Init();

	world = CreateRef<Physics::World>();

	// auto shape = Shape::Create(Shape::Type::Plane);
	// auto plane =
	// 	RigidBody::Create(RigidBody::Type::Static, shape,
	// 		Transform
	// 		{
	// 			.Translation = { 0.0f, -10.0f, 0.0f },
	// 			.Scale = { 100.0f, 0.0f, 100.0f }
	// 		});
	// world->AddActor(plane);
	Ref<Shape> box = Shape::Create(Shape::Type::Box);
	auto body =
		RigidBody::Create(RigidBody::Type::Dynamic, box,
			Transform
			{
				.Translation = { 0.0f, 2.0f, 0.0f }
			});
	Ref<Shape> box2 = Shape::Create(Shape::Type::Box);
	auto floor =
		RigidBody::Create(RigidBody::Type::Dynamic, box2,
			Transform
			{
				.Translation = { 0.0f, -2.0f, 0.0f }
			});
	floor->SetGravity(false);
	world->AddActor(floor);
	world->AddActor(body);

	world->AddContactCallback(
		[](RigidBody* a, RigidBody* b)
		{
			VOLCANICORE_LOG_INFO("Collision");
		});

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
