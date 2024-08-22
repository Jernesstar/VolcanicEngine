#pragma once

#include <OpenGL/Framebuffer.h>

namespace Demo {

class Shadows : public Application {
public:
	Shadows();

	void OnUpdate(TimeStep ts);

private:
	// void RenderScene(Ref<Camera> camera);
	void RenderScene();

	Ref<ShaderPipeline> depthShader;
	Ref<ShaderPipeline> shadowShader;
	Ref<RenderPass> depthPass;
	Ref<RenderPass> shadowPass;

	Ref<Mesh> torch;
	Ref<Mesh> cube;

	Ref<OpenGL::Framebuffer> depthMap;

	// Ref<Camera> depthCamera;
	// Ref<Camera> sceneCamera;
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

	// depthCamera = CreateRef<OrthographicCamera>(20.0f, 20.0f, 1.0f, 7.5f);
	// sceneCamera = CreateRef<StereographicCamera>(75.0f);
}

void Shadows::OnUpdate(TimeStep ts) {
	glm::mat4 lightProjection, lightView, lightSpaceMatrix;
	glm::vec3 lightPos = glm::vec3(-2.0f, 4.0f, -1.0f);
	float near = 1.0f, far = 7.5f;

	lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near, far);
	lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	lightSpaceMatrix = lightProjection * lightView;
	// render scene from light's point of view

	Renderer::StartPass(depthPass);
	{
		Renderer::Clear();

		depthShader->SetMat4("u_LightSpaceMatrix", lightSpaceMatrix);

		RenderScene();
	}
	Renderer::EndPass();

	Renderer::StartPass(shadowPass);
	{
		Renderer::Clear();

		depthMap->Get(AttachmentTarget::Depth).Bind(1);
		shadowShader->SetInt("u_ShadowMap", 1);

		shadowShader->SetMat4("u_LightSpaceMatrix", lightSpaceMatrix);

		RenderScene();
	}
	Renderer::EndPass();

	// RendererAPI::Get()->RenderFramebuffer(depthMap, AttachmentTarget::Depth);
}

void Shadows::RenderScene() {
	// Renderer3D::Begin(camera);

	Renderer3D::DrawMesh(cube, { .Translation = { -2.0f,  0.0f,  0.0f } });
	Renderer3D::DrawMesh(cube, { .Translation = {  2.0f,  0.0f,  0.0f } });
	Renderer3D::DrawMesh(cube, { .Translation = {  0.0f,  0.0f, -2.0f } });
	Renderer3D::DrawMesh(cube, { .Translation = {  0.0f,  0.0f,  2.0f } });

	Renderer3D::DrawMesh(cube, {
									.Translation = { 0.0f, -13.0f, 0.0f},
									.Scale = glm::vec3(20.0f)
							   });

	// Renderer3D::End();
}

}