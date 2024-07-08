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

using namespace VolcaniCore;
using namespace VolcaniCore::OpenGL;

namespace Demo {

class Instancing : public Application {
public:
	Instancing();

	void OnUpdate(TimeStep ts);

private:
	const uint32_t InstanceCount = 500;

	struct Vertex {
		glm::vec3 Position;
		glm::vec2 TextureCoordinate;
	};

	Vertex vertices[8] = 
	{
		{ { -0.5f,  0.5f,  0.5f }, { 0.0f, 1.0f } }, // 0 Front Top Left
		{ {  0.5f,  0.5f,  0.5f }, { 1.0f, 1.0f } }, // 1 Front Top Right
		{ { -0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f } }, // 2 Front Bottom Left
		{ {  0.5f, -0.5f,  0.5f }, { 1.0f, 0.0f } }, // 3 Front Bottom Right

		{ { -0.5f,  0.5f, -0.5f }, { 0.0f, 1.0f } }, // 4 Back Top Left
		{ {  0.5f,  0.5f, -0.5f }, { 1.0f, 1.0f } }, // 5 Back Top Right
		{ { -0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f } }, // 6 Back Bottom Left
		{ {  0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f } }, // 7 Back Bottom Right
	};

	uint32_t indices[36] =
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

	BufferLayout cubeLayout = {
		{ "a_Position",			 BufferDataType::Vec3 },
		{ "a_TextureCoordinate", BufferDataType::Vec2 }
	};

	Ref<ShaderPipeline> instancingShader = ShaderPipeline::Create({
		{ "Sandbox/assets/shaders/Instancing.glsl.vert", ShaderType::Vertex },
		{ "Sandbox/assets/shaders/Instancing.glsl.frag", ShaderType::Fragment }
	});
	Ref<Texture> cubeTexture;
	Ref<Mesh> cube;

	Ref<Camera> camera;
	Ref<CameraController> controller;
};

Instancing::Instancing() {
	EventSystem::RegisterListener<KeyPressedEvent>(
	[](const KeyPressedEvent& event) {
		if(event.Key == Key::Escape)
			Application::Close();
	});

	std::vector<glm::mat4> mats;
	mats.reserve(InstanceCount);

	cubeTexture = Texture::Create("Sandbox/assets/images/stone.png");
	cube = Mesh::Create(vertices, indices,
		Material{
			.Diffuse = texture
		});

	camera = CreateRef<StereographicCamera>(75.0f, 0.01f, 100.0f, 800, 600);
	camera->SetPosition({ 5.0f, 0.0f, 10.0f });
	// camera->SetDirection({ -0.5f, -0.5f, -0.5f });
	controller = CreateRef<CameraController>(camera);
	controller->TranslationSpeed = 0.5f;

	instancingShader->Bind();
	instancingShader->SetTexture("u_Texture", cubeTexture, 0);
}

void Instancing::OnUpdate(TimeStep ts) {
	controller->OnUpdate(ts);
	RendererAPI::Get()->Clear();

	instancingShader->SetMat4("u_ViewProj", camera->GetViewProjection());

	VolcaniCore::Renderer::Clear();

	Renderer3D::Begin(camera);
	float offset = 0.0f;
	for(uint32_t i = 0; i < InstanceCount; i++) {
		Transform t{
			.Translation = { (offset += 5.0f), 0.0f, 0.0f }
		};
		Renderer3D::DrawMesh(cube, t);
	}
	Renderer3D::End();
}


}