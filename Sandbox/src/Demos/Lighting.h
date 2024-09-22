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
	Ref<ShaderPipeline> shader;
	Ref<RenderPass> drawPass;
	Ref<OpenGL::UniformBuffer> buffer;

	Ref<Mesh> cube;
	Ref<Model> torch;
	// Ref<Model> stairs;

	PointLight light;
	SpotLight spot;

	Ref<Camera> camera;
	CameraController controller;
};

Lighting::Lighting() {
	Events::RegisterListener<KeyPressedEvent>(
	[](const KeyPressedEvent& event) {
		if(event.Key == Key::Escape)
			Application::Close();
	});

	UI::Init();

	shader = ShaderPipeline::Create({
		{ "Sandbox/assets/shaders/Lighting.glsl.vert", ShaderType::Vertex },
		{ "Sandbox/assets/shaders/Lighting.glsl.frag", ShaderType::Fragment }
	});
	drawPass = RenderPass::Create("Draw Pass", shader);

	light = PointLight{
		.Constant  = 0.3f,
		.Linear    = 0.0f,
		.Quadratic = 0.032f,
	};
	light.Position = { 0.0f, 1.0f, 0.0f };
	light.Ambient  = { 0.2f, 0.2f, 0.2f };
	light.Diffuse  = { 0.5f, 0.5f, 0.5f };
	light.Specular = { 1.0f, 1.0f, 1.0f };

	spot = SpotLight{
		.Position = { 0.0f, 0.0f, 0.0f },
		.CutoffAngle = 0.3f,
		.Direction = { 0.0f, 0.0f, -1.0f },
		.OuterCutoffAngle = 0.4f
	};

	shader->Bind();

	buffer = CreateRef<OpenGL::UniformBuffer>(
		OpenGL::BufferLayout{
			{ "Position",    OpenGL::BufferDataType::Vec3 },
			{ "CutoffAngle", OpenGL::BufferDataType::Float },
			{ "Direction",   OpenGL::BufferDataType::Vec3 },
			{ "OuterCutoffAngle",  OpenGL::BufferDataType::Float },
		}, 0);

	shader->SetInt("u_PointLightCount", 1);
	shader->SetVec3("u_PointLights[0].Position", light.Position);
	shader->SetVec3("u_PointLights[0].Ambient",  light.Ambient);
	shader->SetVec3("u_PointLights[0].Diffuse",  light.Diffuse);
	shader->SetVec3("u_PointLights[0].Specular", light.Specular);

	shader->SetFloat("u_PointLights[0].Constant",  light.Constant);
	shader->SetFloat("u_PointLights[0].Linear",    light.Linear);
	shader->SetFloat("u_PointLights[0].Quadratic", light.Quadratic);

	cube = Mesh::Create(MeshPrimitive::Cube,
		Material{
			.Diffuse = Texture::Create("Sandbox/assets/images/wood.png"),
			.Specular = Texture::Create("Sandbox/assets/images/wood_specular.png"),
		});
	// torch = Model::Create("Sandbox/assets/models/mc-torch/Torch.obj");
	// stairs = Model::Create("Sandbox/TheMazeIsLava/assets/models/stairs/stairs.obj");

	shader->SetTexture("u_Material.Diffuse",  cube->GetMaterial().Diffuse, 0);
	shader->SetTexture("u_Material.Specular", cube->GetMaterial().Specular, 1);
	shader->SetFloat("u_Material.Shininess", 32.0f);

	shader->Unbind();

	camera = CreateRef<StereographicCamera>(75.0f);
	camera->SetPosition({ 2.5f, 2.5f, 2.5f });
	camera->SetDirection({ -0.5f, -0.5f, -0.5f });

	controller = CameraController{ camera };
	controller.TranslationSpeed = 10.0f;
}

Lighting::~Lighting() {
	UI::Close();
}

void Lighting::OnUpdate(TimeStep ts) {
	UI::Begin();

	controller.OnUpdate(ts);

	spot.Position = camera->GetPosition();
	spot.Direction = camera->GetDirection();

	ImGui::Begin("##Spotlight");
	{
		ImGui::SliderFloat("Cutoff Angle", &spot.CutoffAngle, 0.0f, 1.0f);
		ImGui::SliderFloat("Outer Angle", &spot.OuterCutoffAngle, spot.CutoffAngle, 2.0f);
	}
	ImGui::End();

	buffer->SetData(&spot);

	Renderer::StartPass(drawPass);
	{
		Renderer::Clear();

		Renderer3D::Begin(camera);

		// Renderer3D::DrawModel(torch,
		// {
		// 	.Translation = light.Position - glm::vec3{ 0.0f, 1.0f, 0.0f }
		// });
		Renderer3D::DrawMesh(cube, { .Translation = { -2.0f,  0.0f,  0.0f } });
		Renderer3D::DrawMesh(cube, { .Translation = {  2.0f,  0.0f,  0.0f } });
		Renderer3D::DrawMesh(cube, { .Translation = {  0.0f,  0.0f, -2.0f } });
		Renderer3D::DrawMesh(cube, { .Translation = {  0.0f,  0.0f,  2.0f } });

		// Renderer3D::DrawModel(stairs,
		// {
		// 	.Translation = { 0.0f, -10.0f, 0.0f },
		// });

		Renderer3D::End();
	}
	Renderer::EndPass();

	Renderer::Flush();

	UI::End();
}

}