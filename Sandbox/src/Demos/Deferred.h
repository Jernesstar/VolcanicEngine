#pragma once

namespace Demo {

class Deferred : public Application {
public:
	Deferred();

	void OnUpdate(TimeStep ts) override;

private:
	Ref<Camera> camera;
	CameraController controller;

	Ref<Framebuffer> gBuffer;
	Ref<RenderPass> geometryPass;
	Ref<RenderPass> deferredPass;
};

Deferred::Deferred() {
	Events::RegisterListener<KeyPressedEvent>(
		[](const KeyPressedEvent& event)
		{
			if(event.Key == Key::Escape)
				Application::Close();
		});

	Ref<ShaderPipeline> geom;
	Ref<ShaderPipeline> deferred;
	geom = AssetImporter::GetShader(
	{
		{ "Sandbox/assets/shaders/Geometry.glsl.vert", ShaderType::Vertex },
		{ "Sandbox/assets/shaders/Geometry.glsl.frag", ShaderType::Fragment }
	});
	deferred = AssetImporter::GetShader(
	{
		{ "Sandbox/assets/shaders/Deferred.glsl.vert", ShaderType::Vertex },
		{ "Sandbox/assets/shaders/Deferred.glsl.frag", ShaderType::Fragment }
	});

	Ref<Texture> position = Texture::Create(1920, 1080, Texture::Format::Float);
	Ref<Texture> normal = Texture::Create(1920, 1080, Texture::Format::Float);
	Ref<Texture> color = Texture::Create(1920, 1080, Texture::Format::Float);
	gBuffer = Framebuffer::Create(
		{
			{ AttachmentTarget::Color, { position, normal, color } },
		});
	geometryPass = RenderPass::Create("Geometry", geom);
	geometryPass->SetOutput(gBuffer);
	deferredPass = RenderPass::Create("Deferred", deferred);

	camera = CreateRef<StereographicCamera>(75.0f);
	camera->SetPosition({ 2.5f, 2.5f, 2.5f });
	camera->SetDirection({ -0.5f, -0.5f, -0.5f });
	controller = CameraController{ camera };
}

void Deferred::OnUpdate(TimeStep ts) {

}

}