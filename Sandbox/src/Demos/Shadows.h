#pragma once

#include <OpenGL/Framebuffer.h>

namespace Demo {

class Shadows : public Application {
public:
	Shadows();

	void OnUpdate(TimeStep ts);

private:
	void RenderScene(Ref<Camera> camera);

	Ref<ShaderPipeline> depthShader;
	Ref<ShaderPipeline> shadowShader;
	Ref<RenderPass> depthPass;
	Ref<RenderPass> shadowPass;

	Ref<Mesh> torch;
	Ref<Mesh> cube;

	Ref<OpenGL::Framebuffer> depthMap;

	Ref<Camera> depthCamera;
	Ref<Camera> sceneCamera;
	CameraController controller;
};

Shadows::Shadows() {
	Events::RegisterListener<KeyPressedEvent>(
	[](const KeyPressedEvent& event) {
		if(event.Key == Key::Escape)
			Application::Close();
	});

	depthShader = ShaderPipeline::Create({
		{ "Sandbox/assets/shaders/Depth.glsl.vert", ShaderType::Vertex },
		{ "Sandbox/assets/shaders/Depth.glsl.frag", ShaderType::Fragment }
	});
	shadowShader = ShaderPipeline::Create({
		{ "Sandbox/assets/shaders/Shadow.glsl.vert", ShaderType::Vertex },
		{ "Sandbox/assets/shaders/Shadow.glsl.frag", ShaderType::Fragment }
	});

	std::vector<OpenGL::Attachment> attachments{
		{ AttachmentTarget::Color, OpenGL::AttachmentType::Texture },
		{ AttachmentTarget::Depth, OpenGL::AttachmentType::Texture }
	};
	depthMap = CreateRef<OpenGL::Framebuffer>(1024, 1024, attachments);

	depthPass = RenderPass::Create("Depth Pass", depthShader);
	depthPass->SetOutput(depthMap);

	shadowPass = RenderPass::Create("Shadow Pass", shadowShader);

	cube = Mesh::Create(MeshPrimitive::Cube,
		Material{
			.Diffuse = Texture::Create("Sandbox/assets/images/wood.png")
		});

	depthCamera = CreateRef<OrthographicCamera>(20.0f, 20.0f, 1.0f, 7.5f);
	sceneCamera = CreateRef<StereographicCamera>(75.0f);

	depthCamera->SetPosition({ -2.0f, 4.0f, -1.0f });

	controller = CameraController{ sceneCamera };
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

		// render scene from light's point of view
		RenderScene(depthCamera);
	}
	Renderer::EndPass();

	Renderer::StartPass(shadowPass);
	{
		Renderer::Clear();
		
		Renderer::GetPass()->GetUniforms()
		.Set("u_ShadowMap",
			[&]() -> TextureSlot
			{
				depthMap->Get(AttachmentTarget::Depth).Bind(1);
				shadowShader->SetInt("u_ShadowMap", 1);

				return { nullptr, 0 };
			}
		);

		Renderer::GetPass()->GetUniforms()
		.Set("u_LightSpaceMatrix",
			[&]() -> glm::mat4
			{
				return depthCamera->GetViewProjection();
			});

		Renderer::GetPass()->GetUniforms()
		.Set("u_LightPosition",
			[&]() -> glm::vec3
			{
				return depthCamera->GetPosition();
			});

		RenderScene(sceneCamera);
	}
	Renderer::EndPass();

	RendererAPI::Get()->RenderFramebuffer(depthMap, AttachmentTarget::Depth);
}

void Shadows::RenderScene(Ref<Camera> camera) {
	Renderer3D::Begin(camera);

	Renderer3D::DrawMesh(cube, { .Translation = { -2.0f,  0.0f,  0.0f } });
	Renderer3D::DrawMesh(cube, { .Translation = {  2.0f,  0.0f,  0.0f } });
	Renderer3D::DrawMesh(cube, { .Translation = {  0.0f,  0.0f, -2.0f } });
	Renderer3D::DrawMesh(cube, { .Translation = {  0.0f,  0.0f,  2.0f } });

	Renderer3D::DrawMesh(cube,  {
									.Translation = { 0.0f, -13.0f, 0.0f},
									.Scale = glm::vec3(20.0f)
								});

	Renderer3D::End();
}

}