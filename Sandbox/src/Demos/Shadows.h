#pragma once

namespace Demo {

class Shadows : public Application {
public:
	Shadows();
	~Shadows();

	void OnUpdate(TimeStep ts);

private:
	void RenderScene();

	Ref<RenderPass> depthPass;
	Ref<RenderPass> shadowPass;

	Ref<Framebuffer> src;
	Ref<Texture> depthTexture;
	Ref<Framebuffer> depthMap;

	Ref<Mesh> cube;
	Ref<Mesh> torch;

	Ref<Camera> depthCamera;
	Ref<Camera> sceneCamera;
	CameraController controller;
};

Shadows::Shadows()
	: Application(1920, 1080)
{
	Events::RegisterListener<KeyPressedEvent>(
		[](const KeyPressedEvent& event)
		{
			if(event.Key == Key::Escape)
				Application::Close();
		});

	Ref<ShaderPipeline> depthShader;
	Ref<ShaderPipeline> shadowShader;
	depthShader = ShaderPipeline::Create("Sandbox/assets/shaders", "Depth");
	shadowShader = ShaderPipeline::Create("Sandbox/assets/shaders", "Shadow");

	depthTexture = Texture::Create(2048, 2048, Texture::Format::Depth);
	depthMap = Framebuffer::Create(
		{
			{ AttachmentTarget::Depth, { { depthTexture } } }
		});
	depthPass = RenderPass::Create("Depth", depthShader);
	depthPass->SetOutput(depthMap);
	shadowPass = RenderPass::Create("Shadow", shadowShader);

	cube = Mesh::Create(MeshPrimitive::Cube,
		Material{
			.Diffuse = Texture::Create("Sandbox/assets/images/wood.png")
		});
	torch = Mesh::Create("Sandbox/assets/models/mc-torch/Torch.obj");

	depthCamera = CreateRef<OrthographicCamera>(20.0f, 20.0f, 1.0f, 17.5f);
	sceneCamera = CreateRef<StereographicCamera>(75.0f);
	sceneCamera->Resize(1920, 1080);
	depthCamera->SetPosition({ 6.0f, 4.f, 5.0f });
	depthCamera->SetDirection({ -1.0f, -0.5f, -1.0f });
	// depthCamera->Resize(1920, 1080);

	controller = CameraController{ sceneCamera };
	controller.TranslationSpeed = 5.0f;

	UIRenderer::Init();
}

Shadows::~Shadows() {
	UIRenderer::Close();
}

void Shadows::OnUpdate(TimeStep ts) {
	controller.OnUpdate(ts);

	UIRenderer::BeginFrame();
	ImGui::Begin("Light");
	{
		glm::vec3 position = depthCamera->GetPosition();
		if(ImGui::SliderFloat3("Position", &position.x, -10.0f, 10.0f))
			depthCamera->SetPosition(position);
	}
	ImGui::End();

	Renderer::StartPass(depthPass);
	{
		Renderer::Clear();

		Renderer::GetPass()->GetUniforms()
		.Set("u_LightSpaceMatrix",
			[&]() -> glm::mat4
			{
				return depthCamera->GetViewProjection();
			});

		// Renderer::PushOptions(
		// 	{
		// 		.Cull = RendererAPI::Options::CullingMode::Front
		// 	});

		RenderScene();
	}
	Renderer::EndPass();
	Renderer::Flush();

	// RendererAPI::Get()->RenderFramebuffer(depthMap, AttachmentTarget::Depth);

	Renderer::StartPass(shadowPass);
	{
		Renderer::Clear();
		
		Renderer::GetPass()->GetUniforms()
		.Set("u_ViewProj",
			[&]() -> glm::mat4
			{
				return sceneCamera->GetViewProjection();
			})
		.Set("u_LightSpaceMatrix",
			[&]() -> glm::mat4
			{
				return depthCamera->GetViewProjection();
			})
		.Set("u_CameraPosition",
			[&]() -> glm::vec3
			{
				return sceneCamera->GetPosition();
			})
		.Set("u_LightPosition",
			[&]() -> glm::vec3
			{
				return depthCamera->GetPosition();
			})
		.Set("u_ShadowMap",
			[&]() -> TextureSlot
			{
				return { depthMap->Get(AttachmentTarget::Depth), 1 };
			});

		RenderScene();

		auto pos = depthCamera->GetPosition();
		Renderer3D::DrawMesh(torch,
			{
				.Translation = pos  - glm::vec3{ 0.0f, 0.5f, 0.0f }
			});
		Renderer3D::DrawMesh(Mesh::Create(MeshPrimitive::Cube, glm::vec4(1.0f)),
			{
				.Translation = pos,
				.Scale = glm::vec3(0.5f)
			});
	}
	Renderer::EndPass();
	Renderer::Flush();

	UIRenderer::EndFrame();
}

void Shadows::RenderScene() {
	// Renderer3D::Begin(camera);

	Renderer3D::DrawMesh(cube, { .Translation = { -2.0f,  0.0f,  0.0f } });
	Renderer3D::DrawMesh(cube, { .Translation = {  2.0f,  0.0f,  0.0f } });
	Renderer3D::DrawMesh(cube, { .Translation = {  0.0f,  0.0f, -2.0f } });
	Renderer3D::DrawMesh(cube, { .Translation = {  0.0f,  2.0f,  2.0f } });

	Renderer3D::DrawMesh(cube,  {
									.Translation = { 0.0f, -10.5f, 0.0f },
									.Scale = glm::vec3(20.0f)
								});

	// Renderer3D::End();
}

}