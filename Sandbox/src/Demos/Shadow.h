#pragma once

#include <Core/Application.h>
#include <Core/Log.h>

#include <Events/EventSystem.h>

#include <Object/Mesh.h>
#include <Object/Model.h>
#include <Object/Shader.h>
#include <Object/Texture.h>

#include <Renderer/RendererAPI.h>
#include <Renderer/StereographicCamera.h>
#include <Renderer/OrthographicCamera.h>
#include <Renderer/CameraController.h>

#include <OpenGL/Framebuffer.h>

using namespace VolcaniCore;

namespace Demo {

class Shadow : public Application {
public:
	Shadow();

	void OnUpdate(TimeStep ts);

private:
	void RenderScene();

	Ref<ShaderPipeline> depthShader;
	Ref<ShaderPipeline> shadowShader;
	Ref<RenderPass> depthPass;
	Ref<RenderPass> shadowPass;

	Ref<Camera> depthCamera;
	Ref<Camera> sceneCamera;

	Ref<Mesh> torch;
	Ref<Cube> cube;

	Ref<OpenGL::Framebuffer> depthMap;
};

Shadow::Shadow() {
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
	depthPass->SetOutput(framebuffer);

	shadowPass = RenderPass::Create("Shadow Pass", shadowShader);

	std::vector<OpenGL::Attachment> attachments{
		{ AttachmentTarget::Depth, OpenGL::AttachmentType::Texture },
		{ AttachmentTarget::Color, OpenGL::AttachmentType::None }
	};
	depthMap = CreateRef<OpenGL::Framebuffer>(800, 600, attachments);

	// torch = Model::Create("Sandbox/assets/models/Torch.obj");
	cube = Mesh::Create(MeshPrimitive::Cube,
		Material{
			.Diffuse = Texture::Create("Sandbox/assets/images/wood.png")
		});
}

void Shadow::OnUpdate(TimeStep ts) {
	Renderer::Clear();

	Renderer::StartPass(depthPass)
	{
		RenderScene();
	}
	Renderer::EndPass();

	RendererAPI::Get()->RenderFramebuffer(framebuffer, AttachmentTarget::Depth);

	// Renderer::StartPass(shadowPass)
	// {
	// 	RenderScene();
	// }
	// Renderer::EndPass();
}

void Shadow::RenderScene() {
	Renderer3D::Begin(camera);

	Renderer3D::DrawMesh(cube,
		{
			.Translation = { 0.0f, -20.0f, 0.0f },
			.Scale = glm::vec3(20.0f)
		});
	Renderer3D::DrawMesh(cube, { .Translation = { -2.0f,  0.0f,  0.0f } });
	Renderer3D::DrawMesh(cube, { .Translation = {  2.0f,  0.0f,  0.0f } });
	Renderer3D::DrawMesh(cube, { .Translation = {  0.0f,  0.0f, -2.0f } });
	Renderer3D::DrawMesh(cube, { .Translation = {  0.0f,  0.0f,  2.0f } });

	Renderer3D::End();
}

}