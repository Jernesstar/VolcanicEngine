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
		Near = -100.f;
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
		Projection = glm::ortho(-(float)ViewportWidth  / (R),
								 (float)ViewportWidth  / (R),
								-(float)ViewportHeight / (R),
								 (float)ViewportHeight / (R), Near, Far);
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
	color = Texture::Create(480, 270, Texture::Format::Normal,
							Texture::Sampling::Nearest);
	depth = Texture::Create(1920, 1080, Texture::Format::Depth);
	framebuffer = Framebuffer::Create(
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

	BufferLayout vertexLayout =
		{
			{
				{ "Position", BufferDataType::Vec3 },
				{ "Normal",	  BufferDataType::Vec3 },
				{ "TexCoord", BufferDataType::Vec2 },
			},
			true, // Dynamic
			false // Structure of arrays
		};

	BufferLayout instanceLayout =
		{
			{
				{ "Transform", BufferDataType::Mat4 }
			},
			true, // Dynamic
			true  // Structure of arrays
		};

	DrawBufferSpecification specs
		{
			vertexLayout,
			instanceLayout,
			1'000'000,
			1'000'000,
			100'000
		};

	buffer = RendererAPI::Get()->NewDrawBuffer(specs);
	// buffer->AddIndices(Buffer(cube->GetIndices()));
	// buffer->AddVertices(Buffer(cube->GetVertices()));
	buffer->AddIndices(Buffer(torch->GetIndices()));
	buffer->AddVertices(Buffer(torch->GetVertices()));

	UI::UIRenderer::Init();
}

Cube::~Cube() {
	UI::UIRenderer::Close();
}

void Cube::OnUpdate(TimeStep ts) {
	UI::UIRenderer::BeginFrame();

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
		ImGui::Text("FPS: %0.1f", Renderer::GetDebugInfo().FPS);
		ImGui::Text("Draw Calls: %i", Renderer::GetDebugInfo().DrawCalls);
		ImGui::Text("Indices: %i", Renderer::GetDebugInfo().Indices);
		ImGui::Text("Vertices: %i", Renderer::GetDebugInfo().Vertices);
		ImGui::Text("Instances: %i", Renderer::GetDebugInfo().Instances);
	}
	ImGui::End();

	buffer->Clear(DrawBufferIndex::Instances);

	// auto* command = RendererAPI::Get()->NewDrawCommand(buffer);
	// command->Pipeline = shader;
	// command->Clear = true;
	// command->ViewportWidth = 1920;
	// command->ViewportHeight = 1080;
	// command->UniformData
	// 	.SetInput("u_Diffuse", TextureSlot{ cube->GetMaterial().Diffuse, 0 });
	// command->UniformData
	// 	.SetInput("u_ViewProj", camera->GetViewProjection());
	// command->InstancesIndex = 0;

	// auto& call = command->NewDrawCall();
	// call.Primitive = PrimitiveType::Triangle;
	// call.Partition = PartitionType::Instanced;
	// call.IndexStart = 0;
	// call.VertexStart = 0;
	// call.IndexCount = cube->GetIndices().size();
	// call.VertexCount = cube->GetVertices().size();

	auto* command2 = RendererAPI::Get()->NewDrawCommand(buffer);
	command2->Pipeline = shader;
	command2->Clear = true;
	command2->ViewportWidth = 1920;
	command2->ViewportHeight = 1080;
	command2->UniformData
		.SetInput("u_Diffuse", TextureSlot{ torch->GetMaterial().Diffuse, 0 });
	command2->UniformData
		.SetInput("u_ViewProj", camera->GetViewProjection());
	command2->InstancesIndex = 10'000;

	auto& call2 = command2->NewDrawCall();
	call2.Primitive = PrimitiveType::Triangle;
	call2.Partition = PartitionType::Instanced;
	call2.IndexStart = 0;
	call2.VertexStart = 0;
	call2.IndexCount = torch->GetIndices().size();
	call2.VertexCount = torch->GetVertices().size();

	for(int y = -50; y < 50; y++)
		for(int x = -50; x < 50; x++) {
			Transform transform = { .Translation = { x, 0.0f, y } };
			Transform transform2 = { .Translation = { x, 1.0f, y } };
			auto tr = transform.GetTransform();
			auto tr2 = transform2.GetTransform();

			// command->AddInstance(glm::value_ptr(tr));
			// command->Calls[0].InstanceCount++;

			command2->AddInstance(glm::value_ptr(tr2));
			command2->Calls[0].InstanceCount++;
		}

	// Renderer::StartPass(drawPass);
	// {
	// 	Renderer::Clear();

	// 	Renderer3D::Begin(camera);

	// 	for(int y = -50; y < 50; y++)
	// 		for(int x = -50; x < 50; x++) {
	// 			Renderer3D::DrawMesh(cube, { .Translation = { x, 0.0f, y } });
	// 			Renderer3D::DrawMesh(torch, { .Translation = { x, 1.0f, y } });
	// 		}

	// 	Renderer3D::End();
	// }
	// Renderer::EndPass();

	// Renderer2D::DrawFullscreenQuad(framebuffer, AttachmentTarget::Color);
	RendererAPI::Get()->EndFrame();

	UI::UIRenderer::EndFrame();
}

}