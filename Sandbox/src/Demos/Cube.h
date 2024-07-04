#pragma once

#include <Core/Application.h>
#include <Events/EventSystem.h>

#include <Object/Shader.h>
#include <Object/Texture.h>

#include <Renderer/Renderer.h>
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
	Ref<StereographicCamera> camera;
	// Ref<OrthographicCamera> camera;
	Ref<CameraController> controller;

	Ref<ShaderPipeline> pixelateShader;
	// Ref<ShaderPipeline> cullShader;

	Ref<Texture> stone;

	Ref<RenderPass> drawPass;
	Ref<RenderPass> pixelatePass;

	Ref<OpenGL::Framebuffer> framebuffer;
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

	camera = CreateRef<StereographicCamera>(75.0f, 0.01f, 100.0f, 800, 600);
	// camera = CreateRef<OrthographicCamera>(800, 600, 0.01f, 100.0f);
	camera->SetPosition({ 2.5f, 2.5f, 2.5f });
	camera->SetDirection({ -0.5f, -0.5f, -0.5f });

	controller = CreateRef<CameraController>(camera);

	stone = Texture::Create("Sandbox/assets/images/stone.png");
	pixelateShader = ShaderPipeline::Create({
		{ "Sandbox/assets/shaders/Pixelate.glsl.vert", ShaderType::Vertex },
		{ "Sandbox/assets/shaders/Pixelate.glsl.frag", ShaderType::Fragment }
	});
	// cullShader = ShaderPipeline::Create({
	// 	{ "Sandbox/assets/shaders/Cull.glsl.vert", ShaderType::Vertex },
	// 	{ "Sandbox/assets/shaders/Cull.glsl.geom", ShaderType::Geometry }
	// });

	// cullPass = CreateRef<RenderPass>("Cull Pass", cullShader);
	// drawPass = CreateRef<RenderPass>("Draw Pass", ShaderLibrary::Get("Cube"));
	// pixelatePass = CreateRef<RenderPass>("Pixelate Pass", pixelateShader);

	// drawPass->SetOutput(frameBuffer);
	// pixelatePass->SetInput(frameBuffer);

	std::vector<OpenGL::Attachment> attachments{
		{ AttachmentTarget::Color, OpenGL::AttachmentType::Texture },
		{ AttachmentTarget::Depth, OpenGL::AttachmentType::Texture }
	};
	framebuffer = CreateRef<OpenGL::Framebuffer>(225, 150, attachments);
}

void Cube::OnUpdate(TimeStep ts) {
	// TODO: RenderPass
	
	VolcaniCore::Renderer::Clear();
	controller->OnUpdate(ts);

	VolcaniCore::Renderer::Begin();

	RendererAPI::Get()->Resize(225, 150);
	framebuffer->Bind();
	// VolcaniCore::Renderer::StartPass(drawPass);
	// {
		Renderer3D::Begin(camera);
		VolcaniCore::Renderer::Clear();
		// VolcaniCore::Renderer::DrawMesh(cube);
		Renderer3D::End();
	// }
	// VolcaniCore::Renderer::EndPass();
	framebuffer->Unbind();
	RendererAPI::Get()->Resize(800, 600);

	RendererAPI::Get()->RenderFramebuffer(framebuffer, AttachmentTarget::Color);
	VolcaniCore::Renderer::End();

	// // VolcaniCore::Renderer::StartPass(cullPass);
	// VolcaniCore::Renderer::StartPass(pixelatePass);
}


}