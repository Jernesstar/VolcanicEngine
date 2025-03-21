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

	auto width = Application::GetWindow()->GetWidth();
	auto height = Application::GetWindow()->GetHeight();
	src = Framebuffer::Create(width, height);
	InitMips();

	Ref<ShaderPipeline> shader;
	shader = ShaderPipeline::Create(
		{
			{ "Magma/assets/shaders/Framebuffer.glsl.vert", ShaderType::Vertex },
			{ "Sandbox/assets/shaders/Downsample.glsl.frag", ShaderType::Fragment }
		});
	downsamplePass = RenderPass::Create("Downsample", shader);
	downsamplePass->SetOutput(mips);
	downsamplePass->SetData(Renderer3D::GetMeshBuffer());

	shader = ShaderPipeline::Create(
		{
			{ "Magma/assets/shaders/Framebuffer.glsl.vert", ShaderType::Vertex },
			{ "Sandbox/assets/shaders/Upsample.glsl.frag", ShaderType::Fragment }
		});
	upsamplePass = RenderPass::Create("Upsample", shader);
	upsamplePass->SetOutput(mips);
	upsamplePass->SetData(Renderer3D::GetMeshBuffer());

	shader = ShaderPipeline::Create(
		{
			{ "Magma/assets/shaders/Framebuffer.glsl.vert", ShaderType::Vertex },
			{ "Sandbox/assets/shaders/Bloom.glsl.frag", ShaderType::Fragment }
		});
	bloomPass = RenderPass::Create("Bloom", shader);

	shader = ShaderPipeline::Create("Magma/assets/shaders", "Mesh");
	drawPass = RenderPass::Create("Draw", shader);
	drawPass->SetOutput(src);
	drawPass->SetData(Renderer3D::GetMeshBuffer());

	cube = Mesh::Create(MeshType::Cube, { 0.98f, 0.92f, 0.0f, 1.0f });

	camera = CreateRef<StereographicCamera>(75.0f);
	camera->SetPosition({ 2.5f, 2.5f, 2.5f });
	camera->SetDirection({ -0.5f, -0.5f, -0.5f });
	camera->Resize(width, height);

	controller = CameraController{ camera };
	controller.TranslationSpeed = 5.0f;

	UIRenderer::Init();
}

Bloom::~Bloom() {
	UIRenderer::Close();
}

void Bloom::OnUpdate(TimeStep ts) {
	UIRenderer::BeginFrame();

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
		Renderer::Resize(1920, 1080);

		Renderer3D::Begin(camera);

		Renderer3D::DrawMesh(cube, { .Translation = { -2.0f,  0.0f,  0.0f } });
		Renderer3D::DrawMesh(cube, { .Translation = {  2.0f,  0.0f,  0.0f } });
		Renderer3D::DrawMesh(cube, { .Translation = {  0.0f,  0.0f, -2.0f } });
		Renderer3D::DrawMesh(cube, { .Translation = {  0.0f,  0.0f,  2.0f } });

		Renderer3D::End();
	}
	Renderer::EndPass();

	// Renderer2D::DrawFullscreenQuad(src, AttachmentTarget::Color);

	Renderer::StartPass(downsamplePass);
	{
		Renderer::GetPass()->GetUniforms()
		.Set("u_SrcResolution",
			[&]() -> glm::vec2
			{
				return {
					Application::GetWindow()->GetWidth(),
					Application::GetWindow()->GetHeight()
				};
			})
		.Set("u_SrcTexture",
			[&]() -> TextureSlot
			{
				return { src->Get(AttachmentTarget::Color), 0 };
			});

		Downsample();
	}
	Renderer::EndPass();

	Renderer2D::DrawFullscreenQuad(mips, AttachmentTarget::Color);

	// Renderer::StartPass(upsamplePass);
	// {
	// 	Renderer::GetPass()->GetUniforms()
	// 	.Set("u_FilterRadius",
	// 		[&]() -> float
	// 		{
	// 			return filterRadius;
	// 		});

	// 	Upsample();
	// }
	// Renderer::EndPass();

	// // Renderer2D::DrawFullscreenQuad(mips, AttachmentTarget::Color);

	// Renderer::StartPass(bloomPass);
	// {
	// 	Renderer::GetPass()->GetUniforms()
	// 	.Set("u_Exposure",
	// 		[&]() -> float
	// 		{
	// 			return exposure;
	// 		})
	// 	.Set("u_BloomStrength",
	// 		[&]() -> float
	// 		{
	// 			return bloomStrength;
	// 		})
	// 	.Set("u_BloomTexture",
	// 		[&]() -> TextureSlot
	// 		{
	// 			return { mips->Get(AttachmentTarget::Color), 0 };
	// 		})
	// 	.Set("u_SceneTexture",
	// 		[&]() -> TextureSlot
	// 		{
	// 			return { src->Get(AttachmentTarget::Color), 1 };
	// 		});

	// 	Renderer::Resize(Application::GetWindow()->GetWidth(), Application::GetWindow()->GetHeight());
	// 	Renderer2D::DrawFullscreenQuad(mips, AttachmentTarget::Color);
	// }
	// Renderer::EndPass();

	Renderer::Flush();
	UIRenderer::EndFrame();
}

void Bloom::InitMips() {
	uint32_t windowWidth = Application::GetWindow()->GetWidth();
	uint32_t windowHeight = Application::GetWindow()->GetHeight();

	glm::vec2 mipSize((float)windowWidth, (float)windowHeight);
	glm::ivec2 mipIntSize((uint32_t)windowWidth, (uint32_t)windowHeight);

	List<Ref<Texture>> textures;
	for(uint32_t i = 0; i < mipChainLength; i++) {
		BloomMip mip;

		mipSize *= 0.5f;
		mipIntSize /= 2;
		mip.Size = mipSize;
		mip.IntSize = mipIntSize;

		mip.Sampler =
			Texture::Create(mipIntSize.x, mipIntSize.y, Texture::Format::Float);

		mipChain.Add(mip);
		textures.Add(mip.Sampler);
	}

	mips = Framebuffer::Create(
		{
			{ AttachmentTarget::Color, textures }
		});
}

void Bloom::Downsample() {
	uint32_t i = 0;
	for(const auto& mip : mipChain) {
		auto* command = Renderer::GetCommand();
		command->Outputs = { { AttachmentTarget::Color, i++ } };

		Renderer2D::DrawFullscreenQuad(mips, AttachmentTarget::Color);
		Renderer::PopCommand();

		Renderer::PushCommand();
		auto& uniforms = Renderer::GetPass()->GetUniforms()
		.Clear()
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

		auto* command = Renderer::GetCommand();
		Renderer::GetPass()->GetUniforms()
		.Set("u_SrcTexture",
			[mip]() -> TextureSlot
			{
				return { mip.Sampler, 0 };
			});
		
		command->Outputs = { { AttachmentTarget::Color, i - 1 } };

		Renderer::PushOptions(
		{
			.Blending = BlendingMode::Additive
		});
		Renderer2D::DrawFullscreenQuad(mips, AttachmentTarget::Color);
		Renderer::PopOptions();
		Renderer::PopCommand();

		Renderer::PushCommand();
	}
}

}