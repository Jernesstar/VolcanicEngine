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
	std::vector<Vertex> vertices = 
	{
		{ { -0.5f,  0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, { 0.0f, 1.0f } },	// 0 Front Top Left
		{ {  0.5f,  0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, { 1.0f, 1.0f } },	// 1 Front Top Right
		{ { -0.5f, -0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, { 0.0f, 0.0f } },	// 2 Front Bottom Left
		{ {  0.5f, -0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, { 1.0f, 0.0f } },	// 3 Front Bottom Right

		{ { -0.5f,  0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, { 0.0f, 1.0f } },	// 4 Back Top Left
		{ {  0.5f,  0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, { 1.0f, 1.0f } },	// 5 Back Top Right
		{ { -0.5f, -0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, { 0.0f, 0.0f } },	// 6 Back Bottom Left
		{ {  0.5f, -0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, { 1.0f, 0.0f } },	// 7 Back Bottom Right
	};

	std::vector<uint32_t> indices =
	{
		0, 2, 3,
		3, 1, 0,

		5, 7, 6,
		6, 4, 5,

		4, 6, 2,
		2, 0, 4,

		1, 3, 7,
		7, 5, 1,

		4, 0, 1,
		1, 5, 4,

		7, 3, 2,
		2, 6, 7,
	};

	std::vector<uint32_t> quadIndices =
	{
		0, 2, 3,
		3, 1, 0
	};

	Ref<ShaderPipeline> meshShader;
	Ref<ShaderPipeline> pixelateShader;
	// Ref<ShaderPipeline> cullShader;

	Ref<OpenGL::Framebuffer> framebuffer;

	Ref<RenderPass> drawPass;
	Ref<RenderPass> pixelatePass;

	Ref<StereographicCamera> camera;
	// Ref<OrthographicCamera> camera;
	Ref<CameraController> controller;

	Ref<Mesh> cube;
	Ref<Mesh> quad;
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

	Ref<Texture> texture = Texture::Create("Sandbox/assets/images/stone.png");
	cube = Mesh::Create(vertices, indices,
		Material{
			.Diffuse = texture
		});
	quad = Mesh::Create(vertices, quadIndices,
		Material{
			.Diffuse = texture
		});
}

void Cube::OnUpdate(TimeStep ts) {
	controller->OnUpdate(ts);

	Renderer::StartPass(drawPass);
	{
		VolcaniCore::Renderer::Clear();

		Renderer3D::Begin(camera);
		// Renderer3D::DrawMesh(cube);
		// Renderer3D::DrawMesh(cube, { .Translation = { 10.0f, 1.0f, 1.0f } });
		Renderer3D::DrawMesh(quad);
		Renderer3D::End();
	}
	Renderer::EndPass();

	RendererAPI::Get()->RenderFramebuffer(framebuffer, AttachmentTarget::Color);
}

}