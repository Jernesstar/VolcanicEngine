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

	auto shader = ShaderPipeline::Create("VolcaniCore/assets/shaders", "Mesh");
	auto color = Texture::Create(480, 270, Texture::Format::Normal,
							Texture::Sampling::Nearest);
	auto depth = Texture::Create(1920, 1080, Texture::Format::Depth);
	auto framebuffer = Framebuffer::Create(
		{
			{ AttachmentTarget::Color, { { color } } },
			{ AttachmentTarget::Depth, { { depth } } },
		});

	drawPass = RenderPass::Create("Draw", shader);
	// drawPass->SetOutput(framebuffer);

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

	// BufferLayout vertexLayout =
	// 	{
	// 		{
	// 			{ "Position", BufferDataType::Vec3 },
	// 			{ "Normal",	  BufferDataType::Vec3 },
	// 			{ "TexCoord", BufferDataType::Vec2 },
	// 		},
	// 		true, // Dynamic
	// 		false // Structure of arrays
	// 	};

	// BufferLayout instanceLayout =
	// 	{
	// 		{
	// 			{ "Transform", BufferDataType::Mat4 }
	// 		},
	// 		true, // Dynamic
	// 		true  // Structure of arrays
	// 	};

	// DrawBufferSpecification specs
	// 	{
	// 		vertexLayout,
	// 		instanceLayout,
	// 		1'000'000,
	// 		1'000'000,
	// 		100'000
	// 	};

	// buffer = RendererAPI::Get()->NewDrawBuffer(specs);
	// buffer->AddIndices(Buffer(cube->GetIndices()));
	// buffer->AddVertices(Buffer(cube->GetVertices()));
	// buffer->AddIndices(Buffer(torch->GetIndices()));
	// buffer->AddVertices(Buffer(torch->GetVertices()));

	UIRenderer::Init();
}

Cube::~Cube() {
	UIRenderer::Close();
}

void Cube::OnUpdate(TimeStep ts) {
	UIRenderer::BeginFrame();

	RendererAPI::Get()->StartFrame();

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
		auto info = Renderer::GetDebugInfo();
		ImGui::Text("FPS: %0.1f", info.FPS);
		ImGui::Text("Draw Calls: %li", info.DrawCalls);
		ImGui::Text("Indices: %li", info.Indices);
		ImGui::Text("Vertices: %li", info.Vertices);
		ImGui::Text("Instances: %li", info.Instances);
		ImGui::Text("Triangles: %li", info.Instances * uint64_t(info.Vertices / 3));
	}
	ImGui::End();

	Renderer::StartPass(drawPass);
	{
		Renderer::Clear();

		Renderer3D::Begin(camera);

		for(int y = -100; y < 100; y++)
			for(int x = -100; x < 100; x++) {
				Renderer3D::DrawMesh(cube, { .Translation = { x, 0.0f, y } });
				Renderer3D::DrawMesh(torch, { .Translation = { x, 1.0f, y } });
			}

		Renderer3D::End();
	}
	Renderer::EndPass();

	// Renderer2D::DrawFullscreenQuad(framebuffer, AttachmentTarget::Color);
	Renderer::Flush();

	UIRenderer::EndFrame();
}

}