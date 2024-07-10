#pragma once

#include <Core/Application.h>
#include <Core/Log.h>
#include <Events/EventSystem.h>

#include <Object/Shader.h>
#include <Object/Texture.h>

#include <Renderer/RendererAPI.h>
#include <Renderer/StereographicCamera.h>
#include <Renderer/OrthographicCamera.h>
#include <Renderer/CameraController.h>
#include <Renderer/ShaderLibrary.h>

#include <OpenGL/Renderer.h>

using namespace VolcaniCore;

namespace Demo {

class Instancing : public Application {
public:
	Instancing();

	void OnUpdate(TimeStep ts);

private:
	const uint32_t InstanceCount = 500;

	Ref<Texture> texture;
	Ref<Mesh> cube;
	Ref<ShaderPipeline> instancingShader;

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

	texture = Texture::Create("Sandbox/assets/images/stone.png");
	cube = Mesh::Create(MeshPrimitive::Cube, Material{ .Diffuse = texture });

	instancingShader = ShaderLibrary::Get("Mesh");

	camera = CreateRef<StereographicCamera>(75.0f, 0.01f, 100.0f, 800, 600);
	camera->SetPosition({ 5.0f, 0.0f, 10.0f });
	// camera->SetDirection({ -0.5f, -0.5f, -0.5f });
	controller = CreateRef<CameraController>(camera);
	controller->TranslationSpeed = 0.5f;

	instancingShader->Bind();
}

void Instancing::OnUpdate(TimeStep ts) {
	controller->OnUpdate(ts);
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
	instancingShader->SetTexture("u_Diffuse", texture, 0);
	Renderer3D::End();
}

}