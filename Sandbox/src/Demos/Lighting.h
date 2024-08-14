#pragma once

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

// struct SpotLight : public Light, public DirectionalLight {
// 	float CutoffAngle;
// 	float OuterCutoffAngle;
// };

class Lighting : public Application {
public:
	Lighting();

	void OnUpdate(TimeStep ts);

private:
	Ref<ShaderPipeline> shader;
	Ref<RenderPass> drawPass;

	Ref<Camera> camera;
	Ref<CameraController> controller;

	Ref<Mesh> cube;
	Ref<Model> torch;
	Ref<Model> stairs;

	PointLight light;
};

Lighting::Lighting() {
	Events::RegisterListener<KeyPressedEvent>(
	[](const KeyPressedEvent& event) {
		if(event.Key == Key::Escape)
			Application::Close();
	});

	cube = Mesh::Create(MeshPrimitive::Cube,
	Material{
		.Diffuse = Texture::Create("Sandbox/assets/images/wood.png"),
		.Specular = Texture::Create("Sandbox/assets/images/wood_specular.png"),
	});
	torch = ::Model::Create("Sandbox/assets/models/mc-torch/Torch.obj");
	stairs = ::Model::Create("Sandbox/TheMazeIsLava/assets/models/stairs/stairs.obj");

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
	light.Position = { 0.0f, 1.0f, 0.0f },
	light.Ambient  = { 0.2f, 0.2f, 0.2f },
	light.Diffuse  = { 0.5f, 0.5f, 0.5f },
	light.Specular = { 1.0f, 1.0f, 1.0f },

	shader->Bind();

	shader->SetInt("u_PointLightCount", 1);
	shader->SetVec3("u_PointLights[0].Position", light.Position);
	shader->SetVec3("u_PointLights[0].Ambient",  light.Ambient);
	shader->SetVec3("u_PointLights[0].Diffuse",  light.Diffuse);
	shader->SetVec3("u_PointLights[0].Specular", light.Specular);

	shader->SetFloat("u_PointLights[0].Constant",  light.Constant);
	shader->SetFloat("u_PointLights[0].Linear",    light.Linear);
	shader->SetFloat("u_PointLights[0].Quadratic", light.Quadratic);

	// shader->SetTexture("u_Material.Diffuse", cube->GetMaterial().Diffuse, 0);
	// shader->SetTexture("u_Material.Specular", cube->GetMaterial().Specular, 1);
	// shader->SetFloat("u_Material.Shininess", 32.0f);

	camera = CreateRef<StereographicCamera>(75.0f);
	// camera = CreateRef<OrthographicCamera>(800, 600, 0.1f, 100.0f);
	camera->SetPosition({ 2.5f, 2.5f, 2.5f });
	camera->SetDirection({ -0.5f, -0.5f, -0.5f });
	controller = CreateRef<CameraController>(camera);
}

void Lighting::OnUpdate(TimeStep ts) {
	controller->OnUpdate(ts);

	Renderer::StartPass(drawPass);
	{
		Renderer::Clear();

		Renderer3D::Begin(camera);

		Renderer3D::DrawModel(torch,
		{
			.Translation = light.Position - glm::vec3{ 0.0f, .0f, 0.0f }
		});
		Renderer3D::DrawMesh(cube, { .Translation = { -2.0f,  0.0f,  0.0f } });
		Renderer3D::DrawMesh(cube, { .Translation = {  2.0f,  0.0f,  0.0f } });
		Renderer3D::DrawMesh(cube, { .Translation = {  0.0f,  0.0f, -2.0f } });
		Renderer3D::DrawMesh(cube, { .Translation = {  0.0f,  0.0f,  2.0f } });

		Renderer3D::DrawModel(stairs,
		{
			.Translation = { 0.0f, -3.0f, 0.0f },
		});

		Renderer3D::End();
	}
	Renderer::EndPass();
}

}