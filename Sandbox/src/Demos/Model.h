#pragma once

#include <Core/Application.h>
#include <Events/EventSystem.h>

#include <Object/Model.h>

#include <Renderer/Renderer.h>
#include <Renderer/RendererAPI.h>
#include <Renderer/Renderer3D.h>
#include <Renderer/StereographicCamera.h>
#include <Renderer/CameraController.h>
#include <Renderer/ShaderLibrary.h>

using namespace VolcaniCore;

namespace Demo {

class Model : public Application {
public:
	Model();

	void OnUpdate(TimeStep ts) override;

private:
	Ref<RenderPass> drawPass;

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

	drawPass = CreateRef<RenderPass>("Draw Pass", ShaderLibrary::Get("Mesh"));

	camera = CreateRef<StereographicCamera>(90.0f, 0.1f, 100.0f, 1600, 900);
	camera->SetPosition({ 0.0f, 0.0f, 5.0f });

	controller = CreateRef<CameraController>(camera);

	model = ::Model::Create("Sandbox/assets/models/mc-torch/Torch.obj");
}

void Model::OnUpdate(TimeStep ts) {
	controller->OnUpdate(ts);
	VolcaniCore::Renderer::Clear();

	Renderer::StartPass(drawPass);
	{
		Renderer3D::Begin(camera);

		Renderer3D::DrawModel(model, { .Scale = glm::vec3(20.0f) });

		Renderer3D::End();
	}
	Renderer::EndPass();

}


}