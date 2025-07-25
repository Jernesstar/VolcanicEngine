#pragma once

namespace Demo {

class Raycast : public Application {
public:
	Raycast();
	~Raycast();

	void OnUpdate(TimeStep ts);

private:
	void CreateActors();

	Ref<Physics::World> world;
	RigidBody* selected{ nullptr };

	glm::vec2 pixelSize{ 1.0f/800.0f, 1.0f/600.0f };
	glm::vec3 outlineColor{ 0.0f, 0.0f, 1.0f };

	Ref<RenderPass> drawPass;
	Ref<RenderPass> maskPass;
	Ref<RenderPass> outlinePass;

	Ref<Mesh> cube;

	Ref<Camera> camera;
	CameraController controller;
};

Raycast::Raycast() {
	Events::RegisterListener<KeyPressedEvent>(
		[](const KeyPressedEvent& event)
		{
			if(event.Key == Key::Escape)
				Application::Close();
		});

	Events::RegisterListener<MouseButtonPressedEvent>(
		[this](const MouseButtonPressedEvent& event)
		{
			if(event.Button	!= Mouse::RightButton)
				return;

			auto width = Application::GetWindow()->GetWidth();
			auto height = Application::GetWindow()->GetHeight();
			glm::vec4 originNDC
			{
				(event.x/width - 0.5f) * 2.0f,
				(event.y/height - 0.5f) * 2.0f,
				-1.0f, 1.0f
			};
			glm::vec4 endNDC
			{
				(event.x/width - 0.5f) * 2.0f,
				(event.y/height - 0.5f) * 2.0f,
				1.0f, 1.0f
			};

			glm::mat4 invViewProj = glm::inverse(camera->GetViewProjection());
			glm::vec4 worldStart = invViewProj * originNDC;
			glm::vec4 worldEnd   = invViewProj * endNDC;
			worldStart /= worldStart.w;
			worldEnd   /= worldEnd.w;
			glm::vec3 rayDir = glm::vec3(worldEnd - worldStart);
			float maxDist = 1'000'000.0f;

			auto hitInfo = world->Raycast(worldStart, rayDir, maxDist);
			selected = hitInfo.Actor;
			if(hitInfo)
				VOLCANICORE_LOG_INFO("Hit");
			else
				VOLCANICORE_LOG_INFO("No hit");
		});

	auto draw = AssetImporter::GetShader(
		{
			{ "Magma/assets/shaders/Mesh.glsl.vert", ShaderType::Vertex },
			{ "Magma/assets/shaders/Mesh.glsl.frag", ShaderType::Fragment }
		});
	auto mask = AssetImporter::GetShader(
		{
			{ "Magma/assets/shaders/Mask.glsl.vert", ShaderType::Vertex },
			{ "Magma/assets/shaders/Mask.glsl.frag", ShaderType::Fragment }
		});
	auto outline = AssetImporter::GetShader(
		{
			{ "Magma/assets/shaders/Outline.glsl.vert", ShaderType::Vertex },
			{ "Magma/assets/shaders/Outline.glsl.frag", ShaderType::Fragment }
		});

	drawPass = RenderPass::Create("Draw", draw);
	drawPass->SetData(Renderer3D::GetMeshBuffer());
	maskPass = RenderPass::Create("Mask", mask);
	maskPass->SetData(Renderer3D::GetMeshBuffer());
	outlinePass = RenderPass::Create("Outline", outline);
	outlinePass->SetData(Renderer2D::GetScreenBuffer());

	Ref<Framebuffer> maskBuffer = Framebuffer::Create(800, 600);
	maskPass->SetOutput(maskBuffer);

	camera = CreateRef<StereographicCamera>(75.0f);
	camera->SetPosition({ 0.0f, 0.0f, 3.0f });
	controller = CameraController{ camera };

	cube = Mesh::Create(MeshType::Cube,
		{
			.Diffuse = AssetImporter::GetTexture("Sandbox/assets/images/wood.png")
		});

	Physics::Init();

	world = CreateRef<Physics::World>();
	CreateActors();
}

Raycast::~Raycast() {
	// Physics::Close();
}

void Raycast::CreateActors() {
	Ref<Shape> box = Shape::Create(Shape::Type::Box);

	for(uint32_t x = 0; x < 10; x++) {
		Ref<RigidBody> body =
			RigidBody::Create(RigidBody::Type::Static, box,
				Transform{ .Translation = { x * 2.0f, 0.0f, 0.0f } });

		world->AddActor(body);
	}
}

void Raycast::OnUpdate(TimeStep ts) {
	controller.OnUpdate(ts);
	// controller.GetCamera()->Resize(1920, 1080);
	world->OnUpdate(ts);

	// 1. Draw scene
	Renderer::StartPass(drawPass);
	{
		Renderer::Clear();
		// Renderer::Resize(1920, 1080);
		Renderer3D::Begin(camera);

		for(Ref<RigidBody> actor : *world) {
			// actor->UpdateTransform();
			Renderer3D::DrawMesh(cube, actor->GetTransform());
		}

		Renderer3D::End();
	}
	Renderer::EndPass();

	if(!selected)
		return;

	// 2. Draw selected object into mask texture with color white
	Renderer::StartPass(maskPass, false);
	{
		auto* command = Renderer::NewCommand();
		command->Clear = true;
		command->UniformData
		.SetInput("u_ViewProj", camera->GetViewProjection());
		command->UniformData
		.SetInput("u_Color", glm::vec4(1.0f));

		Renderer3D::DrawMesh(cube, selected->GetTransform(), command);
		Renderer3D::End();
	}
	Renderer::EndPass();

	// 3. Render full-screen quad that creates outline
	Renderer::StartPass(outlinePass);
	{
		auto width = Application::GetWindow()->GetWidth();
		auto height = Application::GetWindow()->GetHeight();

		auto* command = Renderer::GetCommand();
		command->UniformData
		.SetInput("u_PixelSize", 1.0f / glm::vec2(width, height));
		command->UniformData
		.SetInput("u_Color", glm::vec3(0.0f, 0.0f, 1.0f));

		auto mask = maskPass->GetOutput();
		Renderer2D::DrawFullscreenQuad(mask, AttachmentTarget::Color);
	}
	Renderer::EndPass();

#ifdef MAGMA_PHYSICS
	auto scene = world->Get();
	const PxRenderBuffer& rb = scene->getRenderBuffer();

	// for(PxU32 i=0; i < rb.getNbPoints(); i++)
	// {
	// 	const PxDebugPoint& point = rb.getPoints()[i];

	// }

	for(PxU32 i=0; i < rb.getNbLines(); i++)
	{
		const PxDebugLine& line = rb.getLines()[i];

	}
#endif
}

}