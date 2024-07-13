#pragma once

#include <Core/Application.h>
#include <Core/Log.h>

#include <Events/EventSystem.h>

#include <Object/Mesh.h>
#include <Renderer/Model.h>
#include <Object/Shader.h>
#include <Object/Texture.h>

#include <Renderer/RendererAPI.h>
#include <Renderer/StereographicCamera.h>
#include <Renderer/OrthographicCamera.h>
#include <Renderer/CameraController.h>

#include <OpenGL/Framebuffer.h>

using namespace VolcaniCore;

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

	Ref<Camera> depthCamera;
	Ref<Camera> sceneCamera;
};

Shadows::Shadows() {
	EventSystem::RegisterListener<KeyPressedEvent>(
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

	depthPass = RenderPass::Create("Depth Pass", depthShader);
	depthPass->SetOutput(depthMap);
	shadowPass = RenderPass::Create("Shadow Pass", shadowShader);

	std::vector<OpenGL::Attachment> attachments{
		{ AttachmentTarget::Color, OpenGL::AttachmentType::Texture },
		{ AttachmentTarget::Depth, OpenGL::AttachmentType::Texture }
	};
	depthMap = CreateRef<OpenGL::Framebuffer>(1024, 1024, attachments);

	// torch = ::Model::Create("Sandbox/assets/models/mc-torch/Torch.obj");
	cube = Mesh::Create(MeshPrimitive::Cube,
		Material{
			.Diffuse = Texture::Create("Sandbox/assets/images/wood.png")
		});
	
	depthCamera = CreateRef<OrthographicCamera>(20.0f, 20.0f, 1.0f, 7.5f);
	sceneCamera = CreateRef<StereographicCamera>(75.0f, 0.1f, 100.0f, 800, 600);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

void Shadows::OnUpdate(TimeStep ts) {
	VolcaniCore::Renderer::Clear();

	VolcaniCore::Renderer::StartPass(depthPass);
	{
		glClear(GL_DEPTH_BUFFER_BIT);
		glm::mat4 lightProjection, lightView, lightSpaceMatrix;
		glm::vec3 lightPos = glm::vec3(-2.0f, 4.0f, -1.0f);
		float near = 1.0f, far = 7.5f;
		lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near, far);
		lightView = glm::lookAt(lightPos,
								glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
		lightSpaceMatrix = lightProjection * lightView;

		depthShader->SetMat4("u_LightSpaceMatrix", lightSpaceMatrix);

		RenderScene();
	}
	VolcaniCore::Renderer::EndPass();

	RendererAPI::Get()->RenderFramebuffer(depthMap, AttachmentTarget::Depth);
}

void Shadows::RenderScene() {

}

}