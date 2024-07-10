#pragma once

#include <Core/Application.h>
#include <Events/EventSystem.h>

#include <Object/Shader.h>
#include <Object/Texture.h>

#include <Renderer/Renderer.h>
#include <Renderer/RendererAPI.h>
#include <Renderer/Renderer3D.h>
#include <Renderer/ShaderLibrary.h>

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

	Ref<Camera> camera;
	Ref<CameraController> controller;

	Ref<Texture> texture;
	Ref<Mesh> quad;
	Ref<Mesh> cube;
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

	std::vector<OpenGL::Attachment> attachments{
		{ AttachmentTarget::Color, OpenGL::AttachmentType::Texture },
		{ AttachmentTarget::Depth, OpenGL::AttachmentType::Texture }
	};
	framebuffer = CreateRef<OpenGL::Framebuffer>(225, 150, attachments);

	drawPass = CreateRef<RenderPass>("Draw Pass", ShaderLibrary::Get("Mesh"));
	drawPass->SetOutput(framebuffer);

	camera = CreateRef<StereographicCamera>(75.0f, 0.01f, 1000.0f, 800, 600);
	// camera = CreateRef<OrthographicCamera>(800, 600, 0.1f, 100.0f);
	camera->SetPosition({ 2.5f, 2.5f, 2.5f });
	camera->SetDirection({ -0.5f, -0.5f, -0.5f });

	controller = CreateRef<CameraController>(camera);
	controller->TranslationSpeed = 0.008f;

	texture = Texture::Create("Sandbox/assets/images/stone.png");
	cube = Mesh::Create(MeshPrimitive::Cube, Material{ .Diffuse = texture });
	// quad = Mesh::Create(MeshPrimitive::Quad, Material{ .Diffuse = texture });
}

void Cube::OnUpdate(TimeStep ts) {
	controller->OnUpdate(ts);

	Renderer::StartPass(drawPass);
	{
		VolcaniCore::Renderer::Clear();

		Renderer3D::Begin(camera);
		Renderer3D::DrawMesh(cube);
		Renderer3D::DrawMesh(cube, { .Translation = { 0.0f, 2.0f, 0.0f } });

		Renderer3D::End();
	}
	Renderer::EndPass();

	RendererAPI::Get()->RenderFramebuffer(framebuffer, AttachmentTarget::Color);
}

}