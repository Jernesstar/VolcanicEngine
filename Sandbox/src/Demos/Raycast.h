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
	List<Ref<RigidBody>> actors;

	Ref<RigidBody> selected{ nullptr };

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
			float maxDist = 10000.0f;

			auto hitInfo = world->Raycast(worldStart, rayDir, maxDist);
			// selected = hitInfo.Actor;
		});

	auto draw = ShaderPipeline::Create("VolcaniCore/assets/shaders", "Mesh");
	auto mask = ShaderPipeline::Create("Sandbox/assets/shaders", "Mask");
	auto outline = ShaderPipeline::Create("Sandbox/assets/shaders", "Outline");

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

	cube = Mesh::Create(MeshPrimitive::Cube,
		Material
		{
			.Diffuse = Texture::Create("Sandbox/assets/images/wood.png")
		});

	Physics::Init();

	world = CreateRef<Physics::World>();
	CreateActors();

	// auto scene = world->Get();
	// scene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f);
	// scene->setVisualizationParameter(PxVisualizationParameter::eACTOR_AXES, 1.0f);
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

		if(x == 1)
			selected = body;

		// world->AddActor(body);
		actors.Add(body);
	}
}

void Raycast::OnUpdate(TimeStep ts) {
	controller.OnUpdate(ts);
	world->OnUpdate(ts);

	Renderer::Clear();

	// 1. Draw scene without selected object
	Renderer::StartPass(drawPass);
	{
		Renderer3D::Begin(camera);

		for(Ref<RigidBody> actor : actors) {
			// actor->UpdateTransform();

			if(actor == selected)
				continue;

			Renderer3D::DrawMesh(cube, actor->GetTransform());
		}

		Renderer3D::End();
	}
	Renderer::EndPass();

	if(!selected)
		return;

	// 2. Draw selected object into mask texture with color white
	Renderer::StartPass(maskPass);
	{
		Renderer::Clear();

		Renderer::PushCommand();
		Renderer::GetPass()->GetUniforms()
		.Set("u_Color",
			[]() -> glm::vec4
			{
				return glm::vec4(1.0f);
			});
		Renderer::PopCommand();

		Renderer3D::Begin(camera);
		Renderer3D::DrawMesh(cube, selected->GetTransform());
		Renderer3D::End();
	}
	Renderer::EndPass();

	// 3. Render full-screen quad that creates outline
	Renderer::StartPass(outlinePass);
	{
		Renderer::PushCommand();
		Renderer::GetPass()->GetUniforms()
		.Set("u_PixelSize",
			[this]() -> glm::vec2
			{
				return pixelSize;
			});
		Renderer::GetPass()->GetUniforms()
		.Set("u_Color",
			[this]() -> glm::vec3
			{
				return outlineColor;
			});

		auto mask = maskPass->GetOutput();
		Renderer2D::DrawFullscreenQuad(mask, AttachmentTarget::Color);
		Renderer::PopCommand();
	}
	Renderer::EndPass();

	// 4. Draw selected object
	Renderer::StartPass(drawPass);
	{
		Renderer3D::Begin(camera);
		Renderer3D::DrawMesh(cube, selected->GetTransform());
		Renderer3D::End();
	}
	Renderer::EndPass();

	// auto scene = world.Get();
	// const PxRenderBuffer& rb = scene->getRenderBuffer();

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