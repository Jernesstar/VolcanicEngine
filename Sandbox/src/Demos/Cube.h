#pragma once

#include <Core/Application.h>
#include <Events/EventSystem.h>

#include <Object/Shader.h>
#include <Object/Texture.h>

#include <Renderer/Renderer.h>
#include <Renderer/RendererAPI.h>
#include <Renderer/Renderer3D.h>

#include <Renderer/StereographicCamera.h>
#include <Renderer/OrthographicCamera.h>
#include <Renderer/CameraController.h>

#include <OpenGL/Framebuffer.h>

using namespace VolcaniCore;

namespace Demo {

class Cube : public Application {
public:
	Cube();

	void OnUpdate(TimeStep ts);

private:
	Ref<ShaderPipeline> meshShader;
	Ref<ShaderPipeline> pixelateShader;
	// Ref<ShaderPipeline> cullShader;

	Ref<OpenGL::Framebuffer> framebuffer;

	Ref<RenderPass> drawPass;
	Ref<RenderPass> pixelatePass;

	Ref<StereographicCamera> camera;
	// Ref<OrthographicCamera> camera;
	Ref<CameraController> controller;

	// Ref<Mesh> quad;
	// Ref<Mesh> cube;
	Ref<VolcaniCore::Mesh> quad;
	Ref<VolcaniCore::Mesh> cube;
	Ref<Texture> texture;
};

Cube::Cube() {
	EventSystem::RegisterListener<KeyPressedEvent>(
	[](const KeyPressedEvent& event) {
		if(event.Key == Key::Escape)
			Application::Close();
	});
	EventSystem::RegisterListener<WindowResizedEvent>(
	[&](const WindowResizedEvent& event) {
		camera->Resize(event.Width, event.Height);
	});

	meshShader = ShaderPipeline::Create({
		{ "VolcaniCore/assets/shaders/Mesh.glsl.vert", ShaderType::Vertex },
		{ "VolcaniCore/assets/shaders/Mesh.glsl.frag", ShaderType::Fragment }
	});
	std::vector<OpenGL::Attachment> attachments{
		{ AttachmentTarget::Color, OpenGL::AttachmentType::Texture },
		{ AttachmentTarget::Depth, OpenGL::AttachmentType::Texture }
	};
	framebuffer = CreateRef<OpenGL::Framebuffer>(225, 150, attachments);

	// drawPass = CreateRef<RenderPass>("Draw Pass", ShaderLibrary::Get("Mesh"));
	drawPass = CreateRef<RenderPass>("Draw Pass", meshShader);

	drawPass->SetOutput(framebuffer);

	camera = CreateRef<StereographicCamera>(75.0f, 0.01f, 100.0f, 800, 600);
	// camera = CreateRef<OrthographicCamera>(800, 600, 0.01f, 100.0f);
	camera->SetPosition({ 2.5f, 2.5f, 2.5f });
	camera->SetDirection({ -0.5f, -0.5f, -0.5f });

	controller = CreateRef<CameraController>(camera);

	texture = Texture::Create("Sandbox/assets/images/stone.png");
	// cube = Mesh::Create(MeshPrimitive::Cube, Material{ .Diffuse = texture });
	// quad = Mesh::Create(MeshPrimitive::Quad, Material{ .Diffuse = texture });
	cube = VolcaniCore::Mesh::Create(MeshPrimitive::Cube, Material{ .Diffuse = texture });
}

void Cube::OnUpdate(TimeStep ts) {
	controller->OnUpdate(ts);

	Renderer::StartPass(drawPass);
	{
		VolcaniCore::Renderer::Clear();

		Renderer3D::Begin(camera);
		Renderer3D::DrawMesh(cube);
		Renderer3D::DrawMesh(cube, { .Translation = { 0.0f, 2.0f, 0.0f } });
		meshShader->SetTexture("u_Diffuse", texture, 0);
	
		Renderer3D::End();
	}
	Renderer::EndPass();

	RendererAPI::Get()->RenderFramebuffer(framebuffer, AttachmentTarget::Color);
}

}