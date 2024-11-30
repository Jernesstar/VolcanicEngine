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
	~Bloom();

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

	uint32_t mipChainLength = 6;
	float filterRadius	= 0.015f;
	float exposure		= 1.0f;
	float bloomStrength = 0.04f;

	float rotation = 0.0f;

	Ref<Window> window;
};

Bloom::Bloom()
	: Application(1920, 1080)
{
	Events::RegisterListener<KeyPressedEvent>(
		[](const KeyPressedEvent& event)
		{
			if(event.Key == Key::Escape)
				Application::Close();
		});

	window = Application::GetWindow();

	Ref<ShaderPipeline> shader;
	shader = ShaderPipeline::Create({
		{ "VolcaniCore/assets/shaders/Framebuffer.glsl.vert", ShaderType::Vertex },
		{ "Sandbox/assets/shaders/Downsample.glsl.frag", ShaderType::Fragment }
	});
	downsamplePass = RenderPass::Create("Downsample", shader);

	shader = ShaderPipeline::Create({
		{ "VolcaniCore/assets/shaders/Framebuffer.glsl.vert", ShaderType::Vertex },
		{ "Sandbox/assets/shaders/Upsample.glsl.frag", ShaderType::Fragment }
	});
	upsamplePass = RenderPass::Create("Upsample", shader);

	shader = ShaderPipeline::Create({
		{ "VolcaniCore/assets/shaders/Framebuffer.glsl.vert", ShaderType::Vertex },
		{ "Sandbox/assets/shaders/Bloom.glsl.frag", ShaderType::Fragment }
	});
	bloomPass = RenderPass::Create("Bloom", shader);

	shader = ShaderPipeline::Create("VolcaniCore/assets/shaders", "Mesh");
	drawPass = RenderPass::Create("Draw", shader);

	auto width = window->GetWidth();
	auto height = window->GetHeight();
	src = Framebuffer::Create(width, height);
	InitMips();

	drawPass->SetOutput(src);
	downsamplePass->SetOutput(mips);
	upsamplePass->SetOutput(mips);

	cube = Mesh::Create(MeshPrimitive::Cube, { 0.98f, 0.92f, 0.0f, 1.0f });

	camera = CreateRef<StereographicCamera>(75.0f);
	camera->SetPosition({ 2.5f, 2.5f, 2.5f });
	camera->SetDirection({ -0.5f, -0.5f, -0.5f });
	camera->Resize(width, height);

	controller = CameraController{ camera };
	controller.TranslationSpeed = 5.0f;

	UI::UIEngine::Init();
}

Bloom::~Bloom() {
	UI::UIEngine::Close();
}

void Bloom::OnUpdate(TimeStep ts) {
	UI::UIEngine::BeginFrame();

	controller.OnUpdate(ts);

	ImGui::Begin("Parameters");
	{
		ImGui::SliderFloat("Filter radius",  &filterRadius,  0.00001f, 0.1f);
		ImGui::SliderFloat("Exposure",		 &exposure,		 0.00001f, 10.0f);
		ImGui::SliderFloat("Bloom Strength", &bloomStrength, 0.00001f, 1.0f);
	}
	ImGui::End();

	Renderer::StartPass(drawPass);
	{
		Renderer::Clear();
		Renderer3D::Begin(camera);

		// rotation += (float)ts * 0.001;
		// Renderer3D::DrawMesh(cube,
		// 	{
		// 		.Rotation = rotation * glm::vec3{ 3.0f, 2.0f, 0.1f }
		// 	});

		Renderer3D::DrawMesh(cube, { .Translation = { -2.0f,  0.0f,  0.0f } });
		Renderer3D::DrawMesh(cube, { .Translation = {  2.0f,  0.0f,  0.0f } });
		Renderer3D::DrawMesh(cube, { .Translation = {  0.0f,  0.0f, -2.0f } });
		Renderer3D::DrawMesh(cube, { .Translation = {  0.0f,  0.0f,  2.0f } });

		Renderer3D::End();
	}
	Renderer::EndPass();
	Renderer::Flush();

	// RendererAPI::Get()->RenderFramebuffer(src, AttachmentTarget::Color);

	Renderer::StartPass(downsamplePass);
	{
		Renderer::GetPass()->SetGlobalUniforms();

		Renderer::GetDrawCommand().GetUniforms()
		.Set("u_SrcResolution",
			[&]() -> glm::vec2
			{
				return { window->GetWidth(), window->GetHeight() };
			})
		.Set("u_SrcTexture",
			[&]() -> TextureSlot
			{
				src->Bind(AttachmentTarget::Color, 0);
				return { };
			});

		Downsample();
	}
	Renderer::EndPass();
	Renderer::Flush();

	// RendererAPI::Get()->RenderFramebuffer(mips, AttachmentTarget::Color);

	Renderer::StartPass(upsamplePass);
	{
		Renderer::GetPass()->SetGlobalUniforms();

		Renderer::GetPass()->GetUniforms()
		.Set("u_FilterRadius",
			[&]() -> float
			{
				return filterRadius;
			});

		// glBlendFunc(GL_ONE, GL_ONE);
		// glBlendEquation(GL_FUNC_ADD);
		Upsample();
		// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	Renderer::EndPass();
	Renderer::Flush();

	// RendererAPI::Get()->RenderFramebuffer(mips, AttachmentTarget::Color);

	Renderer::StartPass(bloomPass);
	{
		Renderer::GetPass()->SetGlobalUniforms();

		Renderer::GetPass()->GetUniforms()
		.Set("u_Exposure",
			[&]() -> float
			{
				return exposure;
			})
		.Set("u_BloomStrength",
			[&]() -> float
			{
				return bloomStrength;
			});

		auto& uniforms = Renderer::GetDrawCommand().GetUniforms()
		.Set("u_BloomTexture",
			[&]() -> TextureSlot
			{
				mips->Bind(AttachmentTarget::Color, 0);
				return { };
			})
		.Set("u_SceneTexture",
			[&]() -> TextureSlot
			{
				src->Bind(AttachmentTarget::Color, 1);
				return { };
			});

		Renderer::GetPass()->SetUniforms(uniforms);

		RendererAPI::Get()->Resize(window->GetWidth(), window->GetHeight());
		RendererAPI::Get()->RenderFramebuffer(mips, AttachmentTarget::Color);
	}
	Renderer::EndPass();
	Renderer::Flush();

	UI::UIEngine::EndFrame();
}

void Bloom::InitMips() {
	uint32_t windowWidth = window->GetWidth();
	uint32_t windowHeight = window->GetHeight();

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

	mips = Framebuffer::Create(
		{
			{ AttachmentTarget::Color, { { mipChain[0].Sampler } } }
		});
}

void Bloom::Downsample() {
	for(const auto& mip : mipChain) {
		auto& uniforms = Renderer::GetDrawCommand().GetUniforms()
		.Set("WriteTexture",
			[mip, &mips = mips]() -> TextureSlot
			{
				mips->Set(AttachmentTarget::Color, mip.Sampler);
				return { };
			});

		Renderer::GetPass()->SetUniforms(uniforms);

		RendererAPI::Get()->RenderFramebuffer(mips, AttachmentTarget::Color);

		Renderer::Flush();
		Renderer::NewDrawCommand();

		Renderer::GetDrawCommand().GetUniforms()
		.Set("u_SrcResolution",
			[mip]() -> glm::vec2
			{
				return mip.Size;
			})
		.Set("u_SrcTexture",
			[mip]() -> TextureSlot
			{
				return { mip.Sampler, 0 };
			});
	}
}

void Bloom::Upsample() {
	for(uint32_t i = mipChainLength - 1; i > 0; i--) {
		const BloomMip& mip = mipChain[i];
		const BloomMip& nextMip = mipChain[i - 1];

		Renderer::PushOptions(
			{
				.Blending = RendererAPI::Options::BlendingMode::Additive
			});

		auto& uniforms = Renderer::GetDrawCommand().GetUniforms()
		.Set("u_SrcTexture",
			[mip]() -> TextureSlot
			{
				return { mip.Sampler, 0 };
			})
		.Set("WriteTexture",
			[nextMip, &mips = mips]() -> TextureSlot
			{
				mips->Set(AttachmentTarget::Color, nextMip.Sampler);
				return { };
			});

		Renderer::GetPass()->SetUniforms(uniforms);

		RendererAPI::Get()->RenderFramebuffer(mips, AttachmentTarget::Color);

		Renderer::Flush();
		Renderer::NewDrawCommand();
	}
}

}