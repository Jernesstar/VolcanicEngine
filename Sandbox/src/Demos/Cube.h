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
		Near = -1000.f;
		SetDistance(R);
	}
	~IsometricCamera() = default;

	void SetDistance(float r) {
		R = r;
		Position =
			R * Vec3
				{
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
		Projection = glm::ortho(-(float)ViewportWidth  / (R),
								 (float)ViewportWidth  / (R),
								-(float)ViewportHeight / (R),
								 (float)ViewportHeight / (R), Near, Far);
		ViewProjection = Projection * View;
	}
	void CalculateView() override {
		glm::vec3 up = { 0.0f, 1.0f, 0.0f };
		View = glm::lookAt(Position, Position + R*Direction, up);
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
	Ref<RenderPass> drawPass;

	Ref<Mesh> cube;
	Ref<Mesh> torch;

	Ref<Camera> camera;
	CameraController controller;
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

	auto depth = Texture::Create(320, 180, Texture::Format::Depth);
	auto color = Texture::Create(320, 180, Texture::Format::Normal,
								 Texture::Sampling::Nearest);
	auto color2 = Texture::Create(160, 90, Texture::Format::Normal,
								 Texture::Sampling::Nearest);
	auto color3 = Texture::Create(80, 45, Texture::Format::Normal,
								 Texture::Sampling::Nearest);
	auto framebuffer = Framebuffer::Create(
		{
			{ AttachmentTarget::Color, { color, color2, color3 } },
			{ AttachmentTarget::Depth, { depth } },
		});

	auto shader = AssetImporter::GetShader(
		{
			{ "Magma/assets/shaders/Mesh.glsl.vert", ShaderType::Vertex },
			{ "Magma/assets/shaders/Mesh.glsl.frag", ShaderType::Fragment }
		});
	drawPass = RenderPass::Create("Draw", shader);
	drawPass->SetData(Renderer3D::GetMeshBuffer());
	drawPass->SetOutput(framebuffer);

	cube = Mesh::Create(MeshType::Cube,
		{
			.Diffuse = AssetImporter::GetTexture("Sandbox/assets/images/wood.png"),
		});
	torch = AssetImporter::GetMesh("Sandbox/assets/models/mc-torch/Torch.obj");

	camera = CreateRef<IsometricCamera>();
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
	controller.TranslationSpeed = 1.0f;

	UIRenderer::Init();
}

Cube::~Cube() {
	UIRenderer::Close();
}

void Cube::OnUpdate(TimeStep ts) {
	UIRenderer::BeginFrame();

	controller.OnUpdate(ts);

	ImGui::Begin("Lights");
	{
		float r = camera->As<IsometricCamera>()->R;
		if(ImGui::DragFloat("Light.R", &r, 10.0f, 200.0f))
			camera->As<IsometricCamera>()->SetDistance(r);
	}
	ImGui::End();

	ImGui::Begin("Debug");
	{
		auto info = Renderer::GetDebugInfo();
		ImGui::Text("FPS: %0.1f", info.FPS);
		ImGui::Text("Draw Calls: %li", info.DrawCalls);
		ImGui::Text("Indices: %li", info.Indices);
		ImGui::Text("Vertices: %li", info.Vertices);
		ImGui::Text("Instances: %li", info.Instances);
	}
	ImGui::End();

	Renderer::StartPass(drawPass);
	{
		Renderer::Clear();

		Renderer3D::Begin(camera);

		for(int y = -50; y < 50; y++)
			for(int x = -50; x < 50; x++) {
				Renderer3D::DrawMesh(cube, { .Translation = { x, 0.0f, y } });
				Renderer3D::DrawMesh(torch, { .Translation = { x, 1.0f, y } });
			}

		Renderer3D::End();
	}
	Renderer::EndPass();

	Renderer2D::DrawFullscreenQuad(
		drawPass->GetOutput(), AttachmentTarget::Color);
	Renderer::Flush();

	UIRenderer::EndFrame();
}

}