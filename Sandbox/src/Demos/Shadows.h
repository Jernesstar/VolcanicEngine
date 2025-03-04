#pragma once

namespace Demo {

class Shadows : public Application {
public:
	Shadows();
	~Shadows();

	void OnUpdate(TimeStep ts);

private:
	void RenderScene(Ref<Camera> camera);

	Ref<RenderPass> depthPass;
	Ref<RenderPass> shadowPass;
	Ref<RenderPass> depthViewPass;

	Ref<Framebuffer> src;
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

	auto depthViewShader = ShaderPipeline::Create(
		{
			{ "Magma/assets/shaders/Framebuffer.glsl.vert", ShaderType::Vertex },
			{ "Sandbox/assets/shaders/DepthView.glsl.frag", ShaderType::Fragment }
		});
	depthViewPass = RenderPass::Create("DepthView", depthViewShader);
	depthViewPass->SetData(Renderer2D::GetScreenBuffer());

	auto depth = ShaderPipeline::Create("Sandbox/assets/shaders", "Depth");
	auto shadow = ShaderPipeline::Create("Magma/assets/shaders", "Shadow");

	auto depthTexture = Texture::Create(2048, 2048, Texture::Format::Depth);
	depthMap = Framebuffer::Create(
		{
			{ AttachmentTarget::Depth, { { depthTexture } } }
		});

	depthPass = RenderPass::Create("Depth", depth);
	depthPass->SetOutput(depthMap);
	depthPass->SetData(Renderer3D::GetMeshBuffer());
	shadowPass = RenderPass::Create("Shadow", shadow);
	shadowPass->SetData(Renderer3D::GetMeshBuffer());

	cube =
		Mesh::Create(MeshType::Cube,
			Material
			{
				.Diffuse = AssetImporter::GetTexture("Sandbox/assets/images/wood.png")
			});
	torch = AssetImporter::GetMesh("Sandbox/assets/models/mc-torch/Torch.obj");

	depthCamera = CreateRef<OrthographicCamera>(20.0f, 20.0f, 1.0f, 20.0f);
	depthCamera->SetPosition({ -2.0f, 6.0f, -2.0f });
	depthCamera->SetDirection({ 0.0f, -2.0f, 0.0f });
	sceneCamera = CreateRef<StereographicCamera>(75.0f);
	sceneCamera->Resize(1920, 1080);

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

		Renderer::PushCommand();
		Renderer::GetPass()->GetUniforms()
		.Set("u_LightSpaceMatrix",
			[&]() -> glm::mat4
			{
				return depthCamera->GetViewProjection();
			});
		Renderer::PopCommand();

		Renderer::PushOptions(
			{
				.Culling = CullingMode::Front
			});

		RenderScene(depthCamera);

		Renderer::PopOptions();
	}
	Renderer::EndPass();

	// Renderer::StartPass(depthViewPass);
	// {
	// 	Renderer2D::DrawFullscreenQuad(depthMap, AttachmentTarget::Depth);
	// }
	// Renderer::EndPass();

	Renderer::StartPass(shadowPass);
	{
		Renderer::Clear();
		Renderer::Resize(1920, 1080);

		Renderer::PushCommand();
		Renderer::GetPass()->GetUniforms()
		.Set("u_LightSpaceMatrix",
			[&]() -> glm::mat4
			{
				return depthCamera->GetViewProjection();
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
		Renderer::PopCommand();

		RenderScene(sceneCamera);
	}
	Renderer::EndPass();

	Renderer::Flush();
	UIRenderer::EndFrame();
}

void Shadows::RenderScene(Ref<Camera> camera) {
	Renderer3D::Begin(camera);

	Renderer3D::DrawMesh(cube, { .Translation = { -2.0f,  0.0f,  0.0f } });
	Renderer3D::DrawMesh(cube, { .Translation = {  2.0f,  0.0f,  0.0f } });
	Renderer3D::DrawMesh(cube, { .Translation = {  0.0f,  0.0f, -2.0f } });
	Renderer3D::DrawMesh(cube, { .Translation = {  0.0f,  2.0f,  2.0f } });

	Renderer3D::DrawMesh(cube,  {
									.Translation = { 0.0f, -10.5f, 0.0f },
									.Scale = glm::vec3(20.0f)
								});

	if(camera == sceneCamera)
		Renderer3D::DrawMesh(torch,
			{
				.Translation = depthCamera->GetPosition()
			});

	Renderer3D::End();
}

}