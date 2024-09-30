#pragma once

using namespace VolcaniCore;

namespace Demo {

class IsometricCamera : public Camera {
public:
	float R = 5.0f;

public:
	IsometricCamera()
		: Camera(Camera::Type::Ortho)
	{
		Near = -R;
		Position = R * glm::vec3{
			glm::sin(glm::radians(45.0f)),
			glm::sin(glm::radians(35.264f)),
			glm::sin(glm::radians(45.0f))
		};

		Direction = -glm::normalize(Position);

		CalculateProjection();
		CalculateView();
	}
	~IsometricCamera() = default;

	void SetDistance(float r) {
		R = r;
		CalculateProjection();
	}

private:
	void CalculateProjection() override {
		float asp = R * (ViewportWidth/ViewportHeight);
		Projection = glm::ortho(-asp, asp, -R, R, Near, Far);

		ViewProjection = Projection * View;
	}
	void CalculateView() override {
		glm::vec3 up = { 0.0f, 1.0f, 0.0f };
		View = glm::lookAt(Position, Position + Direction, up);
		ViewProjection = Projection * View;
	}
};

class Cube : public Application {
public:
	Cube();
	~Cube();

	void OnUpdate(TimeStep ts);

private:
	Ref<ShaderPipeline> shader;
	Ref<RenderPass> renderPass;

	Ref<Texture> color;
	Ref<Texture> depth;
	Ref<Framebuffer> framebuffer;

	Ref<Mesh> torch;
	Ref<Mesh> cube;

	Ref<Camera> camera;
	CameraController controller;
};

Cube::Cube()
	// : Application(800, 600, "Cube Demo")
	: Application(1920, 1080, "Cube Demo")
{
	Events::RegisterListener<KeyPressedEvent>(
	[](const KeyPressedEvent& event) {
		if(event.Key == Key::Escape)
			Application::Close();
	});

	UI::Init();

	shader = ShaderPipeline::Create("VolcaniCore/assets/shaders", "Mesh");

	// color = Texture::Create(480, 270);
	// depth = Texture::Create(1920, 1080);
	// framebuffer = Framebuffer::Create(
	// 	{
	// 		{ AttachmentTarget::Color, { { color } } },
	// 		{ AttachmentTarget::Depth, { { depth } } },
	// 	});

	renderPass = RenderPass::Create("Render Pass", shader);
	// renderPass->SetOutput(framebuffer);

	torch = Mesh::Create("Sandbox/assets/models/mc-torch/Torch.obj");
	cube = Mesh::Create(MeshPrimitive::Cube,
		Material{
			.Diffuse = Texture::Create("Sandbox/assets/images/wood.png"),
		});

	camera = CreateRef<StereographicCamera>(75.0f);
	// camera = CreateRef<OrthographicCamera>(1920, 1080, 0.1f, 100.0f);
	// camera = CreateRef<IsometricCamera>();
	camera->Resize(1920, 1080);
	controller =
		CameraController(
			MovementControls(
				ControlMap{
					// { Control::Up,   Key::W },
					// { Control::Down, Key::S },
					// { Control::Forward,  Key::Invalid },
					// { Control::Backward, Key::Invalid },
				}
			)
		);

	controller.SetCamera(camera);
	// controller.RotationSpeed = 0.0f;
	controller.TranslationSpeed = 5.0f;
}

Cube::~Cube() {
	UI::Close();
}

void Cube::OnUpdate(TimeStep ts) {
	UI::Begin();

	controller.OnUpdate(ts);

	ImGui::Begin("Lights");
	{
		float r = camera->As<IsometricCamera>()->R;
		ImGui::SliderFloat("Light.R", &r, 0.0f, 100.0f);

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

	Renderer::StartPass(renderPass);
	{
		Renderer::Clear();

		Renderer3D::Begin(camera);

		for(int y = 0; y < 250; y++)
			for(int x = 0; x < 250; x++) {
				Renderer3D::DrawMesh(torch, { .Translation = { x, 1.0f, y } });
				Renderer3D::DrawMesh(cube, { .Translation = { x, 0.0f, y } });
			}

		Renderer3D::End();
	}
	Renderer::EndPass();

	Renderer::Flush();
	// RendererAPI::Get()->RenderFramebuffer(framebuffer, AttachmentTarget::Color);

	UI::End();
}

}