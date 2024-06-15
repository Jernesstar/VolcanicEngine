#pragma once

#include <OpenGL/Renderer.h>
#include <OpenGL/Shader.h>
#include <OpenGL/Model.h>

#include <Core/Application.h>

#include <Events/EventSystem.h>

#include <Renderer/Renderer.h>
#include <Renderer/Model.h>
#include <Renderer/StereographicCamera.h>
#include <Renderer/CameraController.h>

using namespace VolcaniCore;

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

	OpenGL::IndexBuffer* index_buffer = new OpenGL::IndexBuffer(indices);
	OpenGL::VertexBuffer* light_buffer = new OpenGL::VertexBuffer(vertices, l1);
	Ref<OpenGL::VertexArray> light_array = CreateRef<OpenGL::VertexArray>(light_buffer, index_buffer);

	Ref<ShaderPipeline> model_shader = ShaderPipeline::Create({
		{ "Sandbox/assets/shaders/Model.glsl.vert", ShaderType::Vertex },
		{ "Sandbox/assets/shaders/Model.glsl.frag", ShaderType::Fragment }
	});
	Ref<ShaderPipeline> light_shader = ShaderPipeline::Create({
		{ "Sandbox/assets/shaders/Light.glsl.vert", ShaderType::Vertex },
		{ "Sandbox/assets/shaders/Light.glsl.frag", ShaderType::Fragment }
	});

	glm::vec3 light_pos = { 1.2f, 1.0f, 2.0f }, light_color = { 1.0f, 1.0f, 1.0f };
	glm::mat4 light_model{ 1.0f };

	StereographicCamera m_Camera{ 90.0f, 0.1f, 100.0f, 1600, 900 };
	CameraController m_Controller{ m_Camera };

	Ref<VolcaniCore::Model> m_Model;
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
		this->m_Camera.Resize(event.Width, event.Height);
	});

	light_model = glm::translate(light_model, light_pos);
	light_model = glm::scale(light_model, glm::vec3(0.2f));

	m_Camera.SetPosition({ 0.0f, 0.0f, 5.0f });

	model_shader->As<OpenGL::ShaderProgram>()->Bind();
	model_shader->SetMat4("u_Model", glm::mat4(1.0f));
	model_shader->SetVec3("u_LightPosition", light_pos);
	model_shader->SetVec3("u_LightColor", light_color);

	light_shader->As<OpenGL::ShaderProgram>()->Bind();
	light_shader->SetVec3("u_LightColor", light_color);
	light_shader->SetMat4("u_Model", light_model);

	m_Model = VolcaniCore::Model::Create("Sandbox/assets/models/rpg");
}

void Model::OnUpdate(TimeStep ts)
{
	Renderer::Clear();

	m_Controller.OnUpdate(ts);

	light_shader->As<OpenGL::ShaderProgram>()->Bind();
	light_shader->SetMat4("u_ViewProj", m_Camera.GetViewProjection());
	// Application::GetRenderer()->As<OpenGL::Renderer>()->DrawIndexed(light_array);

	model_shader->As<OpenGL::ShaderProgram>()->Bind();
	model_shader->SetMat4("u_ViewProj", m_Camera.GetViewProjection());
	model_shader->SetVec3("u_CameraPosition", m_Camera.GetPosition());
	Renderer::RenderModel(m_Model);
}

}