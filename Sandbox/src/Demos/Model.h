#pragma once

#include <Core/Application.h>
#include <Events/EventSystem.h>

#include <Renderer/Model.h>

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

	drawPass = CreateRef<RenderPass>("Draw Pass", ShaderLibrary::Get("Mesh"));

	camera = CreateRef<StereographicCamera>(90.0f, 0.1f, 100.0f, 800, 600);
	camera->SetPosition({ 0.0f, 0.0f, 5.0f });

	controller = CreateRef<CameraController>(camera);
	controller->TranslationSpeed = 0.05f;

	// TODO: Bug: initializing both causes segfault
	// cube = Mesh::Create(MeshPrimitive::Cube,
	// 	Material{
	// 		.Diffuse = Texture::Create("Sandbox/assets/images/stone.png")
	// 	});
	model = ::Model::Create("Sandbox/assets/models/mc-torch/Torch.obj");
}

void Model::OnUpdate(TimeStep ts) {
	controller->OnUpdate(ts);
	VolcaniCore::Renderer::Clear();

	Renderer::StartPass(drawPass);
	{
		Renderer3D::Begin(camera);

		// Renderer3D::DrawMesh(cube);
		Renderer3D::DrawMesh(model->GetMesh(0), { .Scale = glm::vec3(1.0f) });

		Renderer3D::End();
	}
	Renderer::EndPass();

}


}