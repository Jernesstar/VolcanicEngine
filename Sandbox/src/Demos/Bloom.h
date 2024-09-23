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
	void InitMips(uint32_t mipChainLength);
	void Downsample(float filterRadius);
	void Upsample(float filterRadius);

	Ref<Camera> camera;
	CameraController controller;

	Ref<Mesh> cube;

	Ref<RenderPass> downsamplePass;
	Ref<RenderPass> upsamplePass;
	Ref<RenderPass> bloomPass;
	Ref<RenderPass> drawPass;

	Ref<OpenGL::Framebuffer> src;
	Ref<OpenGL::Framebuffer> mips;
	List<BloomMip> mipChain;
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

	src = CreateRef<OpenGL::Framebuffer>(800, 600);
	drawPass->SetOutput(src);

	uint32_t mipChainLength = 5;
	InitMips(mipChainLength);

	cube = Mesh::Create(MeshPrimitive::Cube, { 1.0f, 0.0f, 0.0f, 1.0f });

	camera = CreateRef<StereographicCamera>(75.0f);
	// camera = CreateRef<OrthographicCamera>(800, 600, 0.1f, 100.0f);
	camera->SetPosition({ 2.5f, 2.5f, 2.5f });
	camera->SetDirection({ -0.5f, -0.5f, -0.5f });
	controller = CameraController{ camera };
}

void Bloom::OnUpdate(TimeStep ts) {
	controller.OnUpdate(ts);

	float filterRadius = 0.005f;

	Renderer::StartPass(drawPass);
	{
		Renderer3D::Begin(camera);

		Renderer3D::DrawMesh(cube);

		Renderer3D::End();
	}
	Renderer::EndPass();

	Renderer::Flush();

	Renderer::StartPass(downsamplePass);
	{
		Renderer::GetDrawCommand().GetUniforms()
		.Set("u_SrcResolution",
		[&]() -> glm::vec2
		{
			auto window = Application::GetWindow();
			return { window->GetWidth(), window->GetHeight() };
		});
		Renderer::GetDrawCommand().GetUniforms()
		.Set("u_SrcTexture",
		[&]() -> int32_t
		{
			src->Get(AttachmentTarget::Color).Bind(0);
			return 0;
		});

		Downsample(filterRadius);
	}
	Renderer::EndPass();

	Renderer::StartPass(upsamplePass);
	{
		Renderer::GetDrawCommand().GetUniforms()
		.Set("u_FilterRadius",
		[&]() -> float
		{
			return filterRadius;
		});

		// Enable additive blending
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
		glBlendEquation(GL_FUNC_ADD);

		Upsample(filterRadius);

		// Disable additive blending
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA); // Restore if this was default
		// glDisable(GL_BLEND);
	}
	Renderer::EndPass();

	Renderer::StartPass(bloomPass);
	{
		RendererAPI::Get()->RenderFramebuffer(mips, AttachmentTarget::Color);
	}
	Renderer::EndPass();
}

void Bloom::InitMips(uint32_t mipChainLength)
{
	uint32_t windowWidth = Application::GetWindow()->GetWidth();
	uint32_t windowHeight = Application::GetWindow()->GetHeight();

	glm::vec2 mipSize((float)windowWidth, (float)windowHeight);
	glm::ivec2 mipIntSize((int)windowWidth, (int)windowHeight);

	for (unsigned int i = 0; i < mipChainLength; i++)
	{
		BloomMip mip;

		mipSize *= 0.5f;
		mipIntSize /= 2;
		mip.Size = mipSize;
		mip.IntSize = mipIntSize;

		// glGenTextures(1, &mip.texture);
		// glBindTexture(GL_TEXTURE_2D, mip.texture);
		// // we are downscaling an HDR color buffer, so we need a float texture format
		// glTexImage2D(GL_TEXTURE_2D, 0, GL_R11F_G11F_B10F,
		//              (int)mipSize.x, (int)mipSize.y,
		//              0, GL_RGB, GL_FLOAT, nullptr);
		// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		// mip.Sampler = Texture::Create(mipSize.x, mipSize.y, Texture::InternalFormat::Float);

		mipChain.emplace_back(mip);
	}

	// mips = Framebuffer::Create(
	// {
	// 	{ AttachmentTarget::Color, { mipChain[0].Sampler } }
	// });
}

void Bloom::Downsample(float filterRadius) {
	for(const auto& mip : mipChain) {
		Renderer::Resize(mip.IntSize.x, mip.IntSize.y);
		mips->Set(AttachmentTarget::Color, mip.Sampler);

		RendererAPI::Get()->RenderFramebuffer(mips, AttachmentTarget::Color);

		Renderer::NewDrawCommand();

		// Set current mip resolution as u_SrcResolution for next iteration
		Renderer::GetDrawCommand().GetUniforms()
		.Set("u_SrcResolution",
		[&]() -> glm::vec2
		{
			return mip.Size;
		});
		// Set current mip as texture input for next iteration
		Renderer::GetDrawCommand().GetUniforms()
		.Set("u_SrcTexture",
		[&]() -> TextureSlot
		{
			return { mip.Sampler, 0 };
		});
	}
}

void Bloom::Upsample(float filterRadius) {
	for(int i = mipChain.size() - 1; i > 0; i--) {
		const BloomMip& mip = mipChain[i];
		const BloomMip& nextMip = mipChain[i-1];

		// Bind viewport and texture from where to read
		Renderer::GetDrawCommand().GetUniforms()
		.Set("u_SrcTexture",
		[&]() -> TextureSlot
		{
			return { mip.Sampler, 0 };
		});

		// Set framebuffer render target (we write to this texture)
		Renderer::Resize(nextMip.IntSize.x, nextMip.IntSize.y);
		mips->Set(AttachmentTarget::Color, nextMip.Sampler);

		RendererAPI::Get()->RenderFramebuffer(mips, AttachmentTarget::Color);
	}
}

}