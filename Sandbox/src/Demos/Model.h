#pragma once

#include <glad/glad.h>

#include <Core/Application.h>
#include <Events/EventSystem.h>

#include <Renderer/Model.h>

#include <Renderer/Renderer.h>
#include <Renderer/RendererAPI.h>
#include <Renderer/Renderer3D.h>
#include <Renderer/StereographicCamera.h>
#include <Renderer/CameraController.h>
#include <Renderer/ShaderLibrary.h>

#include <OpenGL/Mesh.h>

using namespace VolcaniCore;

namespace Demo {

class Model : public Application {
public:
	Model();

	void OnUpdate(TimeStep ts) override;

private:
	Ref<ShaderPipeline> shader;
	Ref<StereographicCamera> camera;
	Ref<CameraController> controller;

	Ref<Mesh> cube;
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

	shader = ShaderPipeline::Create({
		{ "Sandbox/assets/shaders/Model.glsl.vert", ShaderType::Vertex },
		{ "Sandbox/assets/shaders/Model.glsl.frag", ShaderType::Fragment }
	});

	camera = CreateRef<StereographicCamera>(90.0f, 0.01f, 1000.0f, 800, 600);
	camera->SetPosition({ 2.5f, 2.5f, 2.5f });
	camera->SetDirection({ -0.5f, -0.5f, -0.5f });

	controller = CreateRef<CameraController>(camera);

	// TODO: Bug: initializing both causes segfault
	cube = Mesh::Create(MeshPrimitive::Cube,
		Material{
			.Diffuse = Texture::Create("Sandbox/assets/images/stone.png")
		});
	// model = ::Model::Create("Sandbox/assets/models/mc-torch/Torch.obj");
	// model = ::Model::Create("Sandbox/assets/models/sphere/wooden_sphere.obj");
	Ref<Mesh> otherMesh = Mesh::Create(MeshPrimitive::Quad, Material{
		.Diffuse = Texture::Create("Sandbox/assets/images/stone.png")
	});
	shader->Bind();
}

void Model::OnUpdate(TimeStep ts) {
	controller->OnUpdate(ts);
	shader->SetMat4("u_ViewProj", camera->GetViewProjection());
	// shader->SetVec3("u_CameraPosition", camera->GetPosition());

	Renderer::Clear();
	
	Material& material = cube->GetMaterial();
	Transform t = { };
	shader->SetTexture("u_Diffuse", material.Diffuse, 0);
	shader->SetMat4("u_Model", glm::mat4(1.0f));

	auto mesh = cube->As<OpenGL::Mesh>();
	auto count = mesh->m_IndexBuffer->Count;

	mesh->m_VertexArray->Bind();
	glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, 0);
}

}