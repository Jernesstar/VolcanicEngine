#pragma once

namespace Demo {

class Bloom : public Application {
public:
	Bloom();

	void OnUpdate(TimeStep ts);

private:
	Ref<Camera> camera;
	CameraController controller;

	Ref<Mesh> cube;

	Ref<ShaderPipeline> shader;
	Ref<RenderPass> bloomPass;
};

Bloom::Bloom() {
	Events::RegisterListener<KeyPressedEvent>(
	[](const KeyPressedEvent& event) {
		if(event.Key == Key::Escape)
			Application::Close();
	});
	Events::RegisterListener<KeyPressedEvent>(
	[](const KeyPressedEvent& event) {
		if(event.Key == Key::K)
		VOLCANICORE_LOG_INFO("Test Key K");
	});

	shader = ShaderPipeline::Create({
		{ "Sandbox/assets/shaders/Lighting.glsl.vert", ShaderType::Vertex },
		{ "Sandbox/assets/shaders/Lighting.glsl.frag", ShaderType::Fragment }
	});
	bloomPass = RenderPass::Create("Bloom Pass", shader);

	cube = Mesh::Create(MeshPrimitive::Cube, { 1.0f, 0.0f, 0.0f, 1.0f });

	camera = CreateRef<StereographicCamera>(75.0f);
	// camera = CreateRef<OrthographicCamera>(800, 600, 0.1f, 100.0f);
	camera->SetPosition({ 2.5f, 2.5f, 2.5f });
	camera->SetDirection({ -0.5f, -0.5f, -0.5f });
	controller = CameraController{ camera };
}

void Bloom::OnUpdate(TimeStep ts) {
	controller.OnUpdate(ts);

	Renderer::StartPass(bloomPass);
	{
		Renderer3D::Begin(camera);

		Renderer3D::DrawMesh(cube);

		Renderer3D::End();
	}
	Renderer::EndPass();
}

}