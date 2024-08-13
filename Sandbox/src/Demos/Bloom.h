#pragma once

namespace Demo {

class Deferred : public Application {
public:
	Deferred();

	void OnUpdate(TimeStep ts);

private:
	Ref<Camera> camera;
	Ref<CameraController> controller;

	Ref<ShaderPipeline> shader;
	Ref<RenderPass> bloomPass;
};

Deferred::Deferred() {
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

	camera = CreateRef<StereographicCamera>(75.0f);
	// camera = CreateRef<OrthographicCamera>(800, 600, 0.1f, 100.0f);
	camera->SetPosition({ 2.5f, 2.5f, 2.5f });
	camera->SetDirection({ -0.5f, -0.5f, -0.5f });
	controller = CreateRef<CameraController>(camera);
}

void Deferred::OnUpdate(TimeStep ts) {
	Renderer::StartPass(bloomPass);
	{
		Renderer3D::DrawMesh(cube);
	}
	Renderer::EndPass();
}

}