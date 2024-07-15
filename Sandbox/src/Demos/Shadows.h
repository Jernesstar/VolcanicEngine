#pragma once

#include <glad/glad.h>

#include <Core/Application.h>
#include <Core/Log.h>

#include <Events/EventSystem.h>

#include <Object/Mesh.h>
#include <Renderer/Model.h>
#include <Object/Shader.h>
#include <Object/Texture.h>

#include <Renderer/Renderer.h>
#include <Renderer/Renderer3D.h>
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

	torch = ::Model::Create("Sandbox/assets/models/mc-torch/Torch.obj");
	cube = Mesh::Create(MeshPrimitive::Cube,
		Material{
			.Diffuse = Texture::Create("Sandbox/assets/images/wood.png")
		});
	
	depthCamera = CreateRef<OrthographicCamera>(20.0f, 20.0f, 1.0f, 7.5f);
	sceneCamera = CreateRef<StereographicCamera>(75.0f, 0.1f, 100.0f, 800, 600);
}

void Shadows::OnUpdate(TimeStep ts) {
	VolcaniCore::Renderer::Clear();

	Renderer3D::DrawModel(cube, { .Scale =  })
}

void Shadows::RenderScene() {

}

}