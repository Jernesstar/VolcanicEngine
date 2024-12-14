#pragma once

#include <VolcaniCore/Core/Math.h>

using namespace VolcaniCore;

namespace Demo {

class IsometricCamera : public Camera {
public:
	float R = 45.0f;

public:
	IsometricCamera()
		: Camera(Camera::Type::Ortho)
	{
		SetDistance(R);
	}
	~IsometricCamera() = default;

	void SetDistance(float r) {
		R = r;
		Position = R * Vec3{
			glm::sin(glm::radians(45.0f)),
			glm::sin(glm::radians(35.264f)),
			glm::sin(glm::radians(45.0f))
		};
		Direction = -glm::normalize(Position);

		CalculateView();
		CalculateProjection();
	}

private:
	void CalculateProjection() override {
		Projection = glm::ortho(-(float)ViewportWidth  / R,
								 (float)ViewportWidth  / R,
								-(float)ViewportHeight / R,
								 (float)ViewportHeight / R, Near, Far);
		ViewProjection = Projection * View;
	}
	void CalculateView() override {
		glm::vec3 up = { 0.0f, 1.0f, 0.0f };
		View = glm::lookAt(Position, Position + Direction, up);
		ViewProjection = Projection * View;
	}
	void Resize(uint32_t width, uint32_t height) override {
		ViewportWidth = 480;
		ViewportHeight = 270;
	}
};

class Cube : public Application {
public:
	Cube();
	~Cube();

	void OnUpdate(TimeStep ts);

private:
	Ref<ShaderPipeline> shader;
	Ref<RenderPass> drawPass;

	Ref<Texture> color;
	Ref<Texture> depth;
	Ref<Framebuffer> framebuffer;

	Ref<Mesh> cube;
	Ref<Mesh> torch;

	Ref<Camera> camera;
	CameraController controller;

	DrawBuffer* buffer;
};

Cube::Cube()
	: Application(1920, 1080, "Cube Demo")
{
	Events::RegisterListener<KeyPressedEvent>(
		[](const KeyPressedEvent& event)
		{
			if(event.Key == Key::Escape)
				Application::Close();
		});

	shader = ShaderPipeline::Create("VolcaniCore/assets/shaders", "Mesh");
	color = Texture::Create(480, 270,
				Texture::InternalFormat::Normal,
				Texture::SamplingOption::Nearest);
	depth = Texture::Create(1920, 1080, Texture::InternalFormat::Depth);
	framebuffer = Framebuffer::Create(
		{
			{ AttachmentTarget::Color, { { color } } },
			{ AttachmentTarget::Depth, { { depth } } },
		});

	drawPass = RenderPass::Create("Draw", shader);
	drawPass->SetOutput(framebuffer);

	cube = Mesh::Create(MeshPrimitive::Cube,
		Material
		{
			.Diffuse = Texture::Create("Sandbox/assets/images/wood.png"),
		});
	torch = Mesh::Create("Sandbox/assets/models/mc-torch/Torch.obj");

	camera = CreateRef<IsometricCamera>();
	camera->Resize(480, 270);
	controller =
		CameraController(
			MovementControls(
				ControlMap
				{
					{ Control::Up,   Key::W },
					{ Control::Down, Key::S },
					{ Control::Forward,  Key::Invalid },
					{ Control::Backward, Key::Invalid },
				})
			);

	controller.SetCamera(camera);
	controller.RotationSpeed = 0.0f;
	controller.TranslationSpeed = 20.0f;

	UI::UIRenderer::Init();
}

Cube::~Cube() {
	UI::UIRenderer::Close();
}

void Cube::OnUpdate(TimeStep ts) {
	UI::UIRenderer::BeginFrame();

	controller.OnUpdate(ts);

	ImGui::Begin("Lights");
	{
		float r = camera->As<IsometricCamera>()->R;
		if(ImGui::SliderFloat("Light.R", &r, 10.0f, 100.0f))
			camera->As<IsometricCamera>()->SetDistance(r);
	}
	ImGui::End();

	ImGui::Begin("Debug");
	{
		ImGui::Text("FPS: %0.1f", Renderer::GetDebugInfo().FPS);
		ImGui::Text("Draw Calls: %i", Renderer::GetDebugInfo().DrawCalls);
		ImGui::Text("Indices: %i", Renderer::GetDebugInfo().Indices);
		ImGui::Text("Vertices: %i", Renderer::GetDebugInfo().Vertices);
		ImGui::Text("Instances: %i", Renderer::GetDebugInfo().Instances);
	}
	ImGui::End();

	Renderer::StartPass(drawPass);
	{
		Renderer::Clear();

		Renderer3D::Begin(camera);

		for(int y = -50; y < 50; y++)
			for(int x = -50; x < 50; x++)
				Renderer3D::DrawMesh(cube, { .Translation = { x, 0.0f, y } });
		for(int y = -50; y < 50; y++)
			for(int x = -50; x < 50; x++)
				Renderer3D::DrawMesh(torch, { .Translation = { x, 1.0f, y } });

		Renderer3D::End();
	}
	Renderer::EndPass();

	RendererAPI::Get()->EndFrame();
	Renderer2D::DrawFullscreenQuad(framebuffer, AttachmentTarget::Color);
	RendererAPI::Get()->EndFrame();

	UI::UIRenderer::EndFrame();
}

}