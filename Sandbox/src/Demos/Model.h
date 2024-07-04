#pragma once

#include <Core/Application.h>
#include <Events/EventSystem.h>

#include <Object/Model.h>

#include <Renderer/Renderer.h>
#include <Renderer/Renderer3D.h>
#include <Renderer/StereographicCamera.h>
#include <Renderer/CameraController.h>

#include <OpenGL/VertexArray.h>

using namespace VolcaniCore;
using namespace VolcaniCore::OpenGL;

namespace Demo {


class Model : public Application {
public:
	Model();

	void OnUpdate(TimeStep ts) override;

private:
	struct Vertex {
		glm::vec3 Position;
	};

	Vertex vertices[8] = 
	{
		{ { -0.5f,  0.5f,  0.5 } }, // 0 Front Top Left
		{ {  0.5f,  0.5f,  0.5 } }, // 1 Front Top Right
		{ { -0.5f, -0.5f,  0.5 } }, // 2 Front Bottom Left
		{ {  0.5f, -0.5f,  0.5 } }, // 3 Front Bottom Right

		{ { -0.5f,  0.5f, -0.5 } }, // 4 Back Top Left 
		{ {  0.5f,  0.5f, -0.5 } }, // 5 Back Top Right
		{ { -0.5f, -0.5f, -0.5 } }, // 6 Back Bottom Left
		{ {  0.5f, -0.5f, -0.5 } }, // 7 Back Bottom Right
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

	OpenGL::BufferLayout l1 = {
		{ "a_Position", OpenGL::BufferDataType::Vec3, true },
	};

	Ref<IndexBuffer>  indexBuffer = CreateRef<IndexBuffer>(indices);
	Ref<VertexBuffer> lightBuffer = CreateRef<VertexBuffer>(vertices, l1);
	Ref<VertexArray>  lightArray  = CreateRef<OpenGL::VertexArray>();

	Ref<ShaderPipeline> modelShader = ShaderPipeline::Create({
		{ "VolcaniCore/assets/shaders/Model.glsl.vert", ShaderType::Vertex },
		{ "VolcaniCore/assets/shaders/Model.glsl.frag", ShaderType::Fragment }
	});
	// Ref<ShaderPipeline> light_shader = ShaderPipeline::Create({
	// 	{ "Sandbox/assets/shaders/Light.glsl.vert", ShaderType::Vertex },
	// 	{ "Sandbox/assets/shaders/Light.glsl.frag", ShaderType::Fragment }
	// });

	glm::vec3 lightPos = { 1.2f, 1.0f, 2.0f };
	glm::vec3 lightColor = { 1.0f, 1.0f, 1.0f };
	glm::mat4 lightModel = glm::scale(glm::translate(glm::mat4(1.0f), lightPos),
									  glm::vec3(0.2f));

	Ref<StereographicCamera> camera;
	Ref<CameraController> controller;

	Ref<VolcaniCore::Model> model;
};

Model::Model()
{
	EventSystem::RegisterListener<KeyPressedEvent>(
	[](const KeyPressedEvent& event) {
		if(event.Key == Key::Escape)
			Application::Close();
	});
	EventSystem::RegisterListener<WindowResizedEvent>(
	[this](const WindowResizedEvent& event) {
		this->camera->Resize(event.Width, event.Height);
	});

	camera = CreateRef<StereographicCamera>(90.0f, 0.1f, 100.0f, 1600, 900);
	camera->SetPosition({ 0.0f, 0.0f, 5.0f });

	controller = CreateRef<CameraController>(camera);

	// TODO: Move to Renderer
	// modelShader->Bind();
	// modelShader->SetMat4("u_Model", glm::mat4(1.0f));
	// modelShader->SetVec3("u_LightPosition", lightPos);
	// modelShader->SetVec3("u_LightColor", lightColor);

	model = VolcaniCore::Model::Create(
			"Sandbox/assets/models/cat_waiters/Cat Waiter-1.fbx");
}

void Model::OnUpdate(TimeStep ts)
{
	controller->OnUpdate(ts);
	VolcaniCore::Renderer::Clear();

	VolcaniCore::Renderer::Begin();
	Renderer3D::Begin(camera);

	Renderer3D::DrawModel(model);

	Renderer3D::End();
	VolcaniCore::Renderer::End();
}


}