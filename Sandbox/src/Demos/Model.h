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
	Ref<ShaderPipeline> modelShader = ShaderPipeline::Create({
		{ "VolcaniCore/assets/shaders/Model.glsl.vert", ShaderType::Vertex },
		{ "VolcaniCore/assets/shaders/Model.glsl.frag", ShaderType::Fragment }
	});

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
	// model = ::Model::Create("Sandbox/assets/models/mc-torch/Torch.obj");
}

void Model::OnUpdate(TimeStep ts) {
	controller->OnUpdate(ts);
	VolcaniCore::Renderer::Clear();

	// Renderer3D::Begin(camera);

	// Renderer3D::DrawModel(model);

	// Renderer3D::End();
}


}