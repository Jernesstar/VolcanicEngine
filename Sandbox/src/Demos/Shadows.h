#pragma once

namespace Demo {

class Shadows : public Application {
public:
	Shadows();

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

Shadows::Shadows() {
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

	depthTexture = Texture::Create(1024, 1024, Texture::InternalFormat::Depth);
	depthMap = Framebuffer::Create({
			{ AttachmentTarget::Depth, { { depthTexture } } }
		});
	depthPass = RenderPass::Create("Depth Pass", depthShader);
	depthPass->SetOutput(depthMap);
	shadowPass = RenderPass::Create("Shadow Pass", shadowShader);

	cube = Mesh::Create(MeshPrimitive::Cube,
		Material{
			.Diffuse = Texture::Create("Sandbox/assets/images/wood.png")
		});
	torch = Mesh::Create("Sandbox/assets/models/mc-torch/Torch.obj");

	depthCamera = CreateRef<OrthographicCamera>(20.0f, 20.0f, 1.0f, 17.5f);
	sceneCamera = CreateRef<StereographicCamera>(75.0f);
	depthCamera->SetPosition({ -2.0f, 4.0f, -2.0f });

	controller = CameraController{ sceneCamera };
	controller.TranslationSpeed = 5.0f;
}

void Shadows::OnUpdate(TimeStep ts) {
	controller.OnUpdate(ts);

	Renderer::StartPass(depthPass);
	{
		Renderer::Clear();

		Renderer::GetPass()->GetUniforms()
		.Set("u_LightSpaceMatrix",
			[&]() -> glm::mat4
			{
				return depthCamera->GetViewProjection();
			});

		RenderScene();
	}
	Renderer::EndPass();
	Renderer::Flush();

	Renderer::StartPass(shadowPass);
	{
		Renderer::Clear();
		
		Renderer::GetPass()->GetUniforms()
		.Set("u_ViewProj",
			[&]() -> glm::mat4
			{
				return sceneCamera->GetViewProjection();
			});
		Renderer::GetPass()->GetUniforms()
		.Set("u_LightSpaceMatrix",
			[&]() -> glm::mat4
			{
				return depthCamera->GetViewProjection();
			});
		Renderer::GetPass()->GetUniforms()
		.Set("u_CameraPosition",
			[&]() -> glm::vec3
			{
				return sceneCamera->GetPosition();
			});
		Renderer::GetPass()->GetUniforms()
		.Set("u_LightPosition",
			[&]() -> glm::vec3
			{
				return depthCamera->GetPosition();
			});
		Renderer::GetPass()->GetUniforms()
		.Set("u_ShadowMap",
			[&]() -> TextureSlot
			{
				depthMap->Bind(AttachmentTarget::Depth, 1);
				return { };
			});

		RenderScene();

		Renderer3D::DrawMesh(torch,
			{
				.Translation = depthCamera->GetPosition()
							 - glm::vec3{ 0.0f, 1.5f, 0.0f }
			});
	}
	Renderer::EndPass();
}

void Shadows::RenderScene() {
	// Renderer3D::Begin(camera);

	Renderer3D::DrawMesh(cube, { .Translation = { -2.0f,  0.0f,  0.0f } });
	Renderer3D::DrawMesh(cube, { .Translation = {  2.0f,  0.0f,  0.0f } });
	Renderer3D::DrawMesh(cube, { .Translation = {  0.0f,  0.0f, -2.0f } });
	Renderer3D::DrawMesh(cube, { .Translation = {  0.0f,  0.0f,  2.0f } });

	Renderer3D::DrawMesh(cube,  {
									.Translation = { 0.0f, -10.5f, 0.0f },
									.Scale = glm::vec3(20.0f)
								});

	// Renderer3D::End();
}

}