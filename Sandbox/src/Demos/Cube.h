#pragma once

#include <OpenGL/Framebuffer.h>

using namespace VolcaniCore;

namespace Demo {

class IsometricCamera : public Camera {
public:
	IsometricCamera()
		: Camera(Camera::Type::Ortho)
	{
		float r = 1.0f;
		Position = r * glm::vec3{
			glm::sin(glm::radians(45.0f)),
			glm::sin(glm::radians(35.264f)),
			glm::cos(glm::radians(45.0f))
		};

		Direction = r * glm::vec3{
			// 0.0f, 0.0f,
			-glm::sin(glm::radians(45.0f)),
			-glm::sin(glm::radians(90.0f - 35.254f)),
			-1.0f
		}

		CalculateProjection();
		CalculateView();
	}
	~IsometricCamera() = default;

private:
	void CalculateProjection() override {
		// Projection = glm::perspectiveFov(glm::radians(75.0f),
		// 								 (float)m_ViewportWidth,
		// 								 (float)m_ViewportHeight,
		// 								 Near, Far);

		Projection = glm::ortho(-m_ViewportWidth / 2.0f, m_ViewportWidth  / 2.0f,
								-m_ViewportHeight/ 2.0f, m_ViewportHeight / 2.0f,
								Near, Far);
	}
	void CalculateView() override {
		glm::vec3 up = { 0.0f, 1.0f, 0.0f };
		View = glm::lookAt(Position, Position + Direction, up);
		ViewProjection = Projection * View;

		ViewProjection = Projection * View;
	}
};

class Cube : public Application {
public:
	Cube();

	void OnUpdate(TimeStep ts);

private:
	Ref<ShaderPipeline> shader;
	Ref<RenderPass> renderPass;

	Ref<Mesh> torch;
	Ref<Mesh> cube;

	Ref<Camera> camera;
	Ref<CameraController> controller;
};

Cube::Cube() {
	Events::RegisterListener<KeyPressedEvent>(
	[](const KeyPressedEvent& event) {
		if(event.Key == Key::Escape)
			Application::Close();
	});

	shader = ShaderPipeline::Create({
		{ "VolcaniCore/assets/shaders/Mesh.glsl.vert", ShaderType::Vertex },
		{ "VolcaniCore/assets/shaders/Mesh.glsl.frag", ShaderType::Fragment }
	});

	renderPass = RenderPass::Create("Render Pass", shader);

	cube = Mesh::Create(MeshPrimitive::Cube,
		Material{
			.Diffuse = Texture::Create("Sandbox/assets/images/wood.png")
		});

	camera = CreateRef<StereographicCamera>(75.0f);
	// camera = CreateRef<OrthographicCamera>(800, 600, 0.1f, 100.0f);
	// camera = CreateRef<IsometricCamera>();
	// controller = CreateRef<CameraController>(camera);
}

void Cube::OnUpdate(TimeStep ts) {
	// controller->OnUpdate(ts);

	Renderer::StartPass(renderPass);
	{
		Renderer::Clear();

		Renderer3D::Begin(camera);

		Renderer3D::DrawMesh(cube, { .Translation = { 0.0f, 0.0f, -3.0f } });

		Renderer3D::End();
	}
	Renderer::EndPass();
}

}