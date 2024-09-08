#pragma once

static void createActors(Physics::World& world) {
	Shape box(Shape::Type::Box);
	for(uint32_t x = 0; x < 10; x++) {
		Ref<RigidBody> body =
			RigidBody::Create(RigidBody::Type::Static, box,
				Transform{ .Translation = { x * 2.0f, 0.0f, 0.0f } });

		world.AddActor(body);
	}
}

namespace Demo {

class Raycast : public Application {
public:
	Raycast();
	~Raycast();

	void OnUpdate(TimeStep ts);

private:
	Ref<Camera> camera;
	CameraController controller;

	Ref<RenderPass> drawPass;
	Ref<RenderPass> maskPass;
	Ref<RenderPass> outlinePass;

	Ref<Mesh> cube;

	Physics::World world;

	// Ref<RigidBody> selected{ nullptr };
	RigidBody* selected = nullptr;

	glm::vec2 pixelSize{ 1.0f/800.0f, 1.0f/600.0f };
	glm::vec4 outlineColor{ 0.0f, 0.5f, 1.0f, 1.0f };
};

Raycast::Raycast() {
	Events::RegisterListener<KeyPressedEvent>(
	[](const KeyPressedEvent& event) {
		if(event.Key == Key::Escape)
			Application::Close();
	});

	Events::RegisterListener<MouseButtonPressedEvent>(
	[this](const MouseButtonPressedEvent& event) {
		auto width = Application::GetWindow()->GetWidth();
		auto height = Application::GetWindow()->GetHeight();
		glm::vec4 originNDC{
			(event.x/width - 0.5f) * 2.0f,
			(event.y/height - 0.5f) * 2.0f,
			-1.0f, 1.0f
		};
		glm::vec4 endNDC{
			(event.x/width - 0.5f) * 2.0f,
			(event.y/height - 0.5f) * 2.0f,
			1.0f, 1.0f
		};

		glm::mat4 inverseViewProj = glm::inverse(camera->GetViewProjection());
		glm::vec4 worldStart = inverseViewProj * originNDC;
		glm::vec4 worldEnd   = inverseViewProj * endNDC;
		worldStart /= worldStart.w;
		worldEnd   /= worldEnd.w;
		glm::vec3 rayDir = glm::vec3(worldEnd - worldStart);
		float maxDist = 10000.0f;

		auto hitInfo = world.Raycast(worldStart, rayDir, maxDist);
		selected = hitInfo.Actor;
	});

	Ref<ShaderPipeline> drawShader;
	Ref<ShaderPipeline> maskShader;
	Ref<ShaderPipeline> outlineShader;

	drawShader = ShaderPipeline::Create({
		{ "VolcaniCore/assets/shaders/Mesh.glsl.vert", ShaderType::Vertex },
		{ "VolcaniCore/assets/shaders/Mesh.glsl.frag", ShaderType::Fragment }
	});
	maskShader = ShaderPipeline::Create({
		{ "Sandbox/assets/shaders/Mask.glsl.vert", ShaderType::Vertex },
		{ "Sandbox/assets/shaders/Mask.glsl.frag", ShaderType::Fragment }
	});
	outlineShader = ShaderPipeline::Create({
		{ "Sandbox/assets/shaders/Outline.glsl.vert", ShaderType::Vertex },
		{ "Sandbox/assets/shaders/Outline.glsl.frag", ShaderType::Fragment }
	});

	drawPass = RenderPass::Create("Draw Pass", drawShader);
	maskPass = RenderPass::Create("Mask Pass", maskShader);
	outlinePass = RenderPass::Create("Outline Pass", outlineShader);

	outlinePass->GetUniforms()
	.Set<glm::vec2>("u_PixelSize",
		[this]() {
			return pixelSize;
		});
	outlinePass->GetUniforms()
	.Set<glm::vec4>("u_Color",
		[this]() {
			return outlineColor;
		});
	maskPass->GetUniforms()
	.Set<glm::vec4>("u_Color",
		[]() {
			return glm::vec4(1.0f);
		});

	Ref<OpenGL::Framebuffer> mask = CreateRef<OpenGL::Framebuffer>(800, 600);
	maskPass->SetOutput(mask);

	camera = CreateRef<StereographicCamera>(75.0f);
	camera->SetPosition({ 0.0f, 0.0f, 3.0f });
	controller = CameraController{ camera };

	cube = Mesh::Create(MeshPrimitive::Cube,
	Material{
		.Diffuse = Texture::Create("Sandbox/assets/images/wood.png")
		// .Specular = Texture::Create("Sandbox/assets/images/wood_specular.png"),
	});

	createActors(world);

	// auto scene = world.Get();
	// scene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f);
	// scene->setVisualizationParameter(PxVisualizationParameter::eACTOR_AXES, 1.0f);
}

Raycast::~Raycast() {
	// Physics::Close();
}

void Raycast::OnUpdate(TimeStep ts) {
	controller.OnUpdate(ts);
	world.OnUpdate(ts);

	Renderer::Clear();

	// 1. Draw scene without selected object
	Renderer::StartPass(drawPass);
	{
		Renderer3D::Begin(camera);

		for(Ref<RigidBody> actor : world) {
			actor->UpdateTransform();

			if(actor.get() == selected)
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

		Renderer3D::Begin(camera);
		Renderer3D::DrawMesh(cube, selected->GetTransform());
		Renderer3D::End();
	}
	Renderer::EndPass();

	Renderer::EndFrame();

	// 4. Full-screen quad applying some image processing function
	Renderer::StartPass(outlinePass);
	{
		auto mask = maskPass->GetOutput();

		RendererAPI::Get()->RenderFramebuffer(mask, AttachmentTarget::Color);
	}
	Renderer::EndPass();

	// 6. Draw selected object
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