#pragma once

#include <OpenGL/Framebuffer.h>

using namespace VolcaniCore;

namespace Demo {

class IsometricCamera : public Camera {
public:
	IsometricCamera()
		: Camera(CameraType::Ortho)
	{
		float r = 2.0f;
		m_Position = r * glm::vec3{
			glm::sin(glm::radians(45.0f)),
			glm::sin(glm::radians(35.264f)),
			glm::cos(glm::radians(45.0f))
		};

		m_ForwardDirection = r * glm::vec3{
								0.0f,
								glm::sin(glm::radians(90.0f - 35.254f)),
								0.0f
							};
	}
	~IsometricCamera() = default;

private:
	void CalculateProjection() override {
		glm::vec3 up = { 0.0f, 1.0f, 0.0f };
		View = glm::lookAt(Position, Position + ForwardDirection, up);
		ViewProjection = Projection * View;
	}
	void CalculateView() override {
		Projection = glm::perspectiveFov(glm::radians(m_VerticalFOV),
										 (float)m_ViewportWidth,
										 (float)m_ViewportHeight,
										 m_NearClip, m_FarClip);
		ViewProjection = Projection * View;

		// Projection = glm::ortho(-m_ViewportWidth/2.0f, m_ViewportWidth/2.0f,
		// 				-m_ViewportHeight/2.0f, m_ViewportHeight/2.0f,
		// 				m_Near, m_Far);
		// ViewProjection = Projection * View;
	}
};

class Shadows : public Application {
public:
	Shadows();

	void OnUpdate(TimeStep ts);

private:
	Ref<ShaderPipeline> shader;
	Ref<RenderPass> renderPass;

	Ref<Mesh> torch;
	Ref<Mesh> cube;

	Ref<Camera> camera;
};

Shadows::Shadows() {
	EventSystem::RegisterListener<KeyPressedEvent>(
	[](const KeyPressedEvent& event) {
		if(event.Key == Key::Escape)
			Application::Close();
	});

	shader = ShaderPipeline::Create({
		{ "Sandbox/assets/shaders/Mesh.glsl.vert", ShaderType::Vertex },
		{ "Sandbox/assets/shaders/Mesh.glsl.frag", ShaderType::Fragment }
	});

	renderPass = RenderPass::Create("Render Pass", shader);

	cube = Mesh::Create(MeshPrimitive::Cube,
		Material{
			.Diffuse = Texture::Create("Sandbox/assets/images/wood.png")
		});

	// camera = CreateRef<StereographicCamera>(75.0f);
	// camera = CreateRef<OrthographicCamera>(800, 600, 0.1f, 100.0f);
	camera = CreateRef<IsometricCamera>();
	controller = CreateRef<CameraController>(camera);
}

void Shadows::OnUpdate(TimeStep ts) {
	controller->OnUpdate(ts);

	Renderer::StartPass(renderPass);
	{
		Renderer::Clear();

		Renderer3D::Begin(camera);

		Renderer3D::DrawMesh(cube);

		Renderer3D::End();
	}
	Renderer::EndPass();
}

}