#pragma once

#include <OpenGL/UniformBuffer.h>

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
// struct SpotLight : public Light {
// 	glm::vec3 Direction;

// 	float CutoffAngle;
// 	float OuterCutoffAngle;
// };

class Lighting : public Application {
public:
	Lighting();
	~Lighting();

	void OnUpdate(TimeStep ts);

private:
	Ref<RenderPass> lightingPass;
	Ref<OpenGL::UniformBuffer> buffer;

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

	Ref<ShaderPipeline> shader;
	shader = ShaderPipeline::Create({
		{ "Sandbox/assets/shaders/Lighting.glsl.vert", ShaderType::Vertex },
		{ "Sandbox/assets/shaders/Lighting.glsl.frag", ShaderType::Fragment }
	});
	lightingPass = RenderPass::Create("Lighting", shader);

	light = PointLight{
		.Constant  = 0.3f,
		.Linear    = 0.0f,
		.Quadratic = 0.032f,
	};
	light.Position = { 0.0f, 0.001f, 0.0f };
	light.Ambient  = { 0.002f, 0.002f, 0.002f };
	light.Diffuse  = { 0.005f, 0.005f, 0.005f };
	light.Specular = { 0.001f, 0.001f, 0.001f };

	spot = SpotLight{
		.Position = { 0.0f, 0.0f, 0.0f },
		.CutoffAngle = 0.3f,
		.Direction = { 0.0f, 0.0f, -1.0f },
		.OuterCutoffAngle = 0.4f
	};

	buffer = CreateRef<OpenGL::UniformBuffer>(
		BufferLayout{
			{ "Position",    BufferDataType::Vec3 },
			{ "CutoffAngle", BufferDataType::Float },
			{ "Direction",   BufferDataType::Vec3 },
			{ "OuterCutoffAngle", BufferDataType::Float },
		}, 0);

	std::string assetPath = "Sandbox/assets/";
	cube = Mesh::Create(MeshPrimitive::Cube,
		Material{
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

	UI::Init();
}

Lighting::~Lighting() {
	UI::Close();
}

void Lighting::OnUpdate(TimeStep ts) {
	UI::Begin();

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

		Renderer3D::Begin(camera);

		auto cubeUniforms = Uniforms{ }
		.Set("u_Material.Diffuse",
			[&]() -> TextureSlot
			{
				return { cube->GetMaterial().Diffuse, 0 };
			})
		.Set("u_Material.Specular",
			[&]() -> TextureSlot
			{
				return { cube->GetMaterial().Specular, 1 };
			})
		.Set("u_Material.Shininess",
			[&]() -> float
			{
				return 32.0f;
			});

		auto torchUniforms = Uniforms{ }
		.Set("u_Material.Diffuse",
			[&]() -> TextureSlot
			{
				return { torch->GetMaterial().Diffuse, 0 };
			})
		.Set("u_Material.Specular",
			[&]() -> TextureSlot
			{
				return { torch->GetMaterial().Specular, 1 };
			})
		.Set("u_Material.Shininess",
			[&]() -> float
			{
				return 32.0f;
			});

		// Renderer3D::SetMeshUniforms(cube, cubeUniforms);
		// Renderer3D::SetMeshUniforms(torch, torchUniforms);

		Renderer::GetPass()->GetUniforms()
		.Set("u_PointLightCount",
			[&]() -> int32_t
			{
				return 2 * width * 2 * length;
			});

		for(int y = -length; y < length; y++) {
			for(int x = -width; x < width; x++) {
				int i = 2*width * (y + length) + (x + width);
				std::string name = "u_PointLights[" + std::to_string(i) + "].";
		
				// auto& uniforms = Renderer::GetDrawCommand().GetUniforms();

				// uniforms
				// .Set(name + "Position",
				// 	[x=x, y=y]() -> glm::vec3
				// 	{
				// 		return { x, 1.5f, y };
				// 	})
				// .Set(name + "Ambient",
				// 	[&]() -> glm::vec3
				// 	{
				// 		return light.Ambient;
				// 	})
				// .Set(name + "Diffuse",
				// 	[&]() -> glm::vec3
				// 	{
				// 		return light.Diffuse;
				// 	})
				// .Set(name + "Specular",
				// 	[&]() -> glm::vec3
				// 	{
				// 		return light.Specular;
				// 	});

				// uniforms
				// .Set(name + "Constant",
				// 	[&]() -> float
				// 	{
				// 		return light.Constant;
				// 	})
				// .Set(name + "Linear",
				// 	[&]() -> float
				// 	{
				// 		return light.Linear;
				// 	})
				// .Set(name + "Quadratic",
				// 	[&]() -> float
				// 	{
				// 		return light.Quadratic;
				// 	});

				Renderer3D::DrawMesh(cube, { .Translation = { x, 0.0f, y } });
				Renderer3D::DrawMesh(torch, { .Translation = { x, 1.0f, y } });
			}
		}
		Renderer3D::DrawModel(player);

		Renderer3D::End();
	}
	Renderer::EndPass();

	Renderer::Flush();

	UI::End();
}

}