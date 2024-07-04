#pragma once

#include <Core/Application.h>
#include <Core/Log.h>

#include <Events/EventSystem.h>

#include <Renderer/RendererAPI.h>
#include <Object/Shader.h>
#include <Object/Texture.h>
#include <Renderer/StereographicCamera.h>
#include <Renderer/OrthographicCamera.h>
#include <Renderer/CameraController.h>

#include <OpenGL/Renderer.h>
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

	Ref<OpenGL::Framebuffer> frameBuffer;
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
		{ OpenGL::AttachmentTarget::Color, OpenGL::AttachmentType::Texture },
		{ OpenGL::AttachmentTarget::Depth, OpenGL::AttachmentType::Texture }
	};
	frameBuffer = CreateRef<OpenGL::Framebuffer>(225, 150, attachments);
}

void Cube::OnUpdate(TimeStep ts) {
	RendererAPI::Get()->Clear();
	controller->OnUpdate(ts);

	glViewport(0, 0, 225, 150);

	frameBuffer->Bind();
	// RendererAPI::Get()->StartPass(drawPass);
	// {
		RendererAPI::Get()->Begin(camera);
		RendererAPI::Get()->Clear();
		RendererAPI::Get()->Draw3DCube(stone);
		RendererAPI::Get()->End();
	// }
	// RendererAPI::Get()->EndPass();
	frameBuffer->Unbind();

	glViewport(0, 0, 800, 600);

	RendererAPI::Get()->RenderFramebuffer(frameBuffer);

	// // RendererAPI::Get()->StartPass(cullPass);
	// RendererAPI::Get()->StartPass(pixelatePass);
}

}