#pragma once

namespace Demo {

struct BloomMip {
	glm::vec2 Size;
	glm::ivec2 IntSize;
	Ref<Texture> Sampler;
};

class Bloom : public Application {
public:
	Bloom();

	void OnUpdate(TimeStep ts);

private:
	void InitMips();
	void Downsample();
	void Upsample();

	Ref<Camera> camera;
	CameraController controller;

	Ref<Mesh> cube;

	Ref<RenderPass> downsamplePass;
	Ref<RenderPass> upsamplePass;
	Ref<RenderPass> bloomPass;
	Ref<RenderPass> drawPass;

	Ref<Framebuffer> src;
	Ref<Framebuffer> mips;
	List<BloomMip> mipChain;
	uint32_t mipChainLength = 5;
	Ref<Texture> test;
};

Bloom::Bloom() {
	Events::RegisterListener<KeyPressedEvent>(
	[](const KeyPressedEvent& event) {
		if(event.Key == Key::Escape)
			Application::Close();
	});

	Ref<ShaderPipeline> shader;
	shader = ShaderPipeline::Create({
		{ "VolcaniCore/assets/shaders/Framebuffer.glsl.vert", ShaderType::Vertex },
		{ "Sandbox/assets/shaders/Downsample.glsl.frag", ShaderType::Fragment }
	});
	downsamplePass = RenderPass::Create("Downsample Pass", shader);

	shader = ShaderPipeline::Create({
		{ "VolcaniCore/assets/shaders/Framebuffer.glsl.vert", ShaderType::Vertex },
		{ "Sandbox/assets/shaders/Upsample.glsl.frag", ShaderType::Fragment }
	});
	upsamplePass = RenderPass::Create("Upsample Pass", shader);

	shader = ShaderPipeline::Create({
		{ "VolcaniCore/assets/shaders/Framebuffer.glsl.vert", ShaderType::Vertex },
		{ "Sandbox/assets/shaders/Bloom.glsl.frag", ShaderType::Fragment }
	});
	bloomPass = RenderPass::Create("Draw Pass", shader);

	shader = ShaderPipeline::Create({
		{ "VolcaniCore/assets/shaders/Mesh.glsl.vert", ShaderType::Vertex },
		{ "VolcaniCore/assets/shaders/Mesh.glsl.frag", ShaderType::Fragment }
	});
	drawPass = RenderPass::Create("Draw Pass", shader);

	auto window = Application::GetWindow();
	src = Framebuffer::Create(window->GetWidth(), window->GetHeight());

	InitMips();
	drawPass->SetOutput(mips);

	cube = Mesh::Create(MeshPrimitive::Cube, { 0.98f, 0.92f, 0.0f, 1.0f });

	camera = CreateRef<StereographicCamera>(75.0f);
	camera->SetPosition({ 2.5f, 2.5f, 2.5f });
	camera->SetDirection({ -0.5f, -0.5f, -0.5f });

	controller = CameraController{ camera };
	controller.TranslationSpeed = 5.0f;
}

void Bloom::OnUpdate(TimeStep ts) {
	controller.OnUpdate(ts);

	float filterRadius = 0.005f;

	mips->Set(AttachmentTarget::Color, mipChain[4].Sampler);

	Renderer::StartPass(drawPass);
	{
		Renderer::Clear();
		Renderer3D::Begin(camera);

		Renderer3D::DrawMesh(cube);

		Renderer3D::End();
	}
	Renderer::EndPass();

	Renderer::Flush();

	RendererAPI::Get()->RenderFramebuffer(mips, AttachmentTarget::Color);

	// Renderer::StartPass(downsamplePass);
	// {
	// 	Renderer::GetDrawCommand().GetUniforms()
	// 	.Set("u_SrcResolution",
	// 	[&]() -> glm::vec2
	// 	{
	// 		auto window = Application::GetWindow();
	// 		return { window->GetWidth(), window->GetHeight() };
	// 	});
	// 	Renderer::GetDrawCommand().GetUniforms()
	// 	.Set("u_SrcTexture",
	// 	[&]() -> TextureSlot
	// 	{
	// 		src->Bind(AttachmentTarget::Color, 0);
	// 		return { };
	// 	});

	// 	Downsample();
	// }
	// Renderer::EndPass();

	// Renderer::Flush();

	// RendererAPI::Get()->RenderFramebuffer(mips, AttachmentTarget::Color);

	// Renderer::StartPass(upsamplePass);
	// {
	// 	Renderer::GetPass()->GetUniforms()
	// 	.Set("u_FilterRadius",
	// 	[&]() -> float
	// 	{
	// 		return filterRadius;
	// 	});

	// 	// Enable additive blending
	// 	glEnable(GL_BLEND);
	// 	glBlendFunc(GL_ONE, GL_ONE);
	// 	glBlendEquation(GL_FUNC_ADD);

	// 	Upsample();

	// 	// Disable additive blending
	// 	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA); // Restore if this was default
	// 	// glDisable(GL_BLEND);
	// }
	// Renderer::EndPass();

	// Renderer::StartPass(bloomPass);
	// {
	// 	Renderer::GetPass()->GetUniforms()
	// 	.Set("u_SceneTexture",
	// 	[&]() -> TextureSlot
	// 	{
	// 		src->Bind(AttachmentTarget::Color, 1);
	// 		return { };
	// 	});

	// 	RendererAPI::Get()->RenderFramebuffer(mips, AttachmentTarget::Color);
	// }
	// Renderer::EndPass();
}

void Bloom::InitMips() {
	uint32_t windowWidth = Application::GetWindow()->GetWidth();
	uint32_t windowHeight = Application::GetWindow()->GetHeight();

	glm::vec2 mipSize((float)windowWidth, (float)windowHeight);
	glm::ivec2 mipIntSize((uint32_t)windowWidth, (uint32_t)windowHeight);

	for(uint32_t i = 0; i < mipChainLength; i++) {
		BloomMip mip;

		mipSize *= 0.5f;
		mipIntSize /= 2;
		mip.Size = mipSize;
		mip.IntSize = mipIntSize;

		mip.Sampler = Texture::Create(mipIntSize.x, mipIntSize.y,
										Texture::InternalFormat::Float);

		mipChain.push_back(mip);
	}

	mips = Framebuffer::Create({
			{ AttachmentTarget::Color, { mipChain[0].Sampler } }
		});
}

void Bloom::Downsample() {
	for(const auto& mip : mipChain) {
		Renderer::Resize(mip.IntSize.x, mip.IntSize.y);

		Renderer::GetDrawCommand().GetUniforms()
		.Set("Sampler",
		[mip, &mips = mips]() -> TextureSlot
		{
			mips->Set(AttachmentTarget::Color, mip.Sampler);
			return { };
		});
		RendererAPI::Get()->RenderFramebuffer(mips, AttachmentTarget::Color);

		Renderer::Flush();

		Renderer::NewDrawCommand();

		// Set current mip resolution as u_SrcResolution for next iteration
		Renderer::GetDrawCommand().GetUniforms()
		.Set("u_SrcResolution",
		[mip]() -> glm::vec2
		{
			return mip.Size;
		});
		// Set current mip as texture input for next iteration
		Renderer::GetDrawCommand().GetUniforms()
		.Set("u_SrcTexture",
		[mip]() -> TextureSlot
		{
			return { mip.Sampler, 0 };
		});
	}
}

void Bloom::Upsample() {
	for(int i = mipChain.size() - 1; i > 0; i--) {
		const BloomMip& mip = mipChain[i];
		const BloomMip& nextMip = mipChain[i-1];

		// Bind viewport and texture from where to read
		Renderer::GetDrawCommand().GetUniforms()
		.Set("u_SrcTexture",
		[mip]() -> TextureSlot
		{
			return { mip.Sampler, 0 };
		});

		// Set framebuffer render target (we write to this texture)
		Renderer::Resize(nextMip.IntSize.x, nextMip.IntSize.y);
		mips->Set(AttachmentTarget::Color, nextMip.Sampler);

		RendererAPI::Get()->RenderFramebuffer(mips, AttachmentTarget::Color);

		Renderer::Flush();

		Renderer::NewDrawCommand();
	}
}

}