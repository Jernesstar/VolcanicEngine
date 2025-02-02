#pragma once

#include <Graphics/UniformBuffer.h>

namespace Demo {

struct Light {
	glm::vec3 Position;
	glm::vec3 Ambient;
	glm::vec3 Diffuse;
	glm::vec3 Specular;
};

struct DirectionalLight : public Light {
	glm::vec3 Direction;
};

struct PointLight : public Light {
	float Constant;
	float Linear;
	float Quadratic;
};

struct SpotLight {
	glm::vec3 Position;
	float CutoffAngle;
	glm::vec3 Direction;
	float OuterCutoffAngle;
};

class Lighting : public Application {
public:
	Lighting();
	~Lighting();

	void OnUpdate(TimeStep ts);

private:
	Ref<RenderPass> lightingPass;
	Ref<UniformBuffer> buffer;

	Ref<Mesh> cube;
	Ref<Mesh> torch;
	Ref<Model> player;

	PointLight light;
	SpotLight spot;

	Ref<Camera> camera;
	CameraController controller;

	int width, length;
};

Lighting::Lighting() {
	Events::RegisterListener<KeyPressedEvent>(
		[](const KeyPressedEvent& event)
		{
			if(event.Key == Key::Escape)
				Application::Close();
		});

	auto shader = ShaderPipeline::Create("Sandbox/assets/shaders", "Lighting");
	lightingPass = RenderPass::Create("Lighting", shader);
	lightingPass->SetData(Renderer3D::GetMeshBuffer());

	// light = 
	// 	PointLight
	// 	{
	// 		.Constant  = 0.3f,
	// 		.Linear    = 0.0f,
	// 		.Quadratic = 0.032f,
	// 	};
	// light.Position = { 0.0f, 0.001f, 0.0f };
	// light.Ambient  = { 0.002f, 0.002f, 0.002f };
	// light.Diffuse  = { 0.005f, 0.005f, 0.005f };
	// // light.Specular = { 0.001f, 0.001f, 0.001f };

	spot = SpotLight
		{
			.Position = { 0.0f, 0.0f, 0.0f },
			.CutoffAngle = 0.3f,
			.Direction = { 0.0f, 0.0f, -1.0f },
			.OuterCutoffAngle = 0.4f
		};

	buffer =
		UniformBuffer::Create(
			BufferLayout
			{
				{ "Position",    BufferDataType::Vec3 },
				{ "CutoffAngle", BufferDataType::Float },
				{ "Direction",   BufferDataType::Vec3 },
				{ "OuterCutoffAngle", BufferDataType::Float },
			});

	std::string assetPath = "Sandbox/assets/";
	cube =
		Mesh::Create(MeshPrimitive::Cube,
			Material
			{
				.Diffuse = Texture::Create(assetPath + "images/wood.png"),
				.Specular = Texture::Create(assetPath + "images/wood_specular.png"),
			});
	torch = Mesh::Create(assetPath + "models/mc-torch/Torch.obj");
	player = Model::Create(assetPath + "models/player/Knight_Golden_Male.obj");

	camera = CreateRef<StereographicCamera>(75.0f);
	camera->SetPosition({ 2.5f, 2.5f, 2.5f });
	camera->SetDirection({ -0.5f, -0.5f, -0.5f });

	controller = CameraController{ camera };
	controller.TranslationSpeed = 10.0f;

	width = 5;
	length = 5;

	UIRenderer::Init();
}

Lighting::~Lighting() {
	UIRenderer::Close();
}

void Lighting::OnUpdate(TimeStep ts) {
	UIRenderer::BeginFrame();

	controller.OnUpdate(ts);

	spot.Position = camera->GetPosition();
	spot.Direction = camera->GetDirection();

	ImGui::Begin("Spotlight");
	{
		ImGui::SliderFloat("Cutoff Angle", &spot.CutoffAngle, 0.0f, 1.0f);
		ImGui::SliderFloat("Outer Angle", &spot.OuterCutoffAngle, spot.CutoffAngle, 2.0f);
	}
	ImGui::End();

	buffer->SetData(&spot);

	Renderer::StartPass(lightingPass);
	{
		Renderer::Clear();
		Renderer::Resize(800, 600);

		Renderer3D::Begin(camera);

		auto* command = Renderer::GetCommand();
		command->UniformData
		.SetInput(UniformSlot{ buffer, "", 0 });
		command->UniformData
		.SetInput("u_PointLightCount", 2 * width * 2 * length);

		auto* cubeCommand = Renderer::NewCommand();
		cubeCommand->UniformData
		.SetInput("u_Material.Diffuse",
				  TextureSlot{ cube->GetMaterial().Diffuse, 0 });
		cubeCommand->UniformData
		.SetInput("u_Material.Specular",
				  TextureSlot{ cube->GetMaterial().Specular, 1 });
		cubeCommand->UniformData
		.SetInput("u_Material.Shininess", 32.0f);

		auto* torchCommand = Renderer::NewCommand();
		torchCommand->UniformData
		.SetInput("u_Material.Diffuse",
				  TextureSlot{ torch->GetMaterial().Diffuse, 0 });
		torchCommand->UniformData
		.SetInput("u_Material.Specular",
				  TextureSlot{ torch->GetMaterial().Specular, 1 });
		torchCommand->UniformData
		.SetInput("u_Material.Shininess", 32.0f);

		for(int y = -50; y < 50; y++)
			for(int x = -50; x < 50; x++) {
				Renderer3D::DrawMesh(cube, { .Translation = { x, 0.0f, y } }, cubeCommand);
				Renderer3D::DrawMesh(torch, { .Translation = { x, 1.0f, y } }, torchCommand);
			}

		Renderer3D::DrawModel(player);

		Renderer3D::End();
	}
	Renderer::EndPass();

	Renderer::Flush();

	UIRenderer::EndFrame();
}

}