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

	uint32_t mipChainLength = 7;
	float filterRadius	= 0.005f;
	float exposure		= 1.0f;
	float bloomStrength = 0.04f;
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
			{ "Magma/assets/shaders/Downsample.glsl.frag", ShaderType::Fragment }
		});
	downsamplePass = RenderPass::Create("Downsample", shader, mips);
	downsamplePass->SetData(Renderer2D::GetScreenBuffer());

	shader = ShaderPipeline::Create(
		{
			{ "Magma/assets/shaders/Framebuffer.glsl.vert", ShaderType::Vertex },
			{ "Magma/assets/shaders/Upsample.glsl.frag", ShaderType::Fragment }
		});
	upsamplePass = RenderPass::Create("Upsample", shader, mips);
	upsamplePass->SetData(Renderer2D::GetScreenBuffer());

	shader = ShaderPipeline::Create(
		{
			{ "Magma/assets/shaders/Framebuffer.glsl.vert", ShaderType::Vertex },
			{ "Magma/assets/shaders/Bloom.glsl.frag", ShaderType::Fragment }
		});
	bloomPass = RenderPass::Create("Bloom", shader);
	bloomPass->SetData(Renderer2D::GetScreenBuffer());

	shader = ShaderPipeline::Create("Magma/assets/shaders", "Mesh");
	drawPass = RenderPass::Create("Draw", shader, src);
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

		Renderer3D::Begin(camera);

		Renderer3D::DrawMesh(cube, { .Translation = { -2.0f,  0.0f,  0.0f } });
		Renderer3D::DrawMesh(cube, { .Translation = {  2.0f,  0.0f,  0.0f } });
		Renderer3D::DrawMesh(cube, { .Translation = {  0.0f,  0.0f, -2.0f } });
		Renderer3D::DrawMesh(cube, { .Translation = {  0.0f,  0.0f,  2.0f } });
		for(int i = -20; i < 20; i++) {
			Renderer3D::DrawMesh(cube, { .Translation = { i, i, 0.0f } });
			Renderer3D::DrawMesh(cube, { .Translation = { 0.0f, i, i } });
			Renderer3D::DrawMesh(cube, { .Translation = { i, 0.0f, i } });
		}

		Renderer3D::End();
	}
	Renderer::EndPass();

	// Renderer2D::DrawFullscreenQuad(src, AttachmentTarget::Color);

	Renderer::StartPass(downsamplePass, false);
	{
		Downsample();
	}
	Renderer::EndPass();

	Renderer::StartPass(upsamplePass, false);
	{
		Upsample();
	}
	Renderer::EndPass();

	// // Renderer2D::DrawFullscreenQuad(mips, AttachmentTarget::Color);

	Renderer::StartPass(bloomPass, false);
	{
		auto* command = Renderer::NewCommand();
		command->ViewportWidth = Application::GetWindow()->GetWidth();
		command->ViewportHeight = Application::GetWindow()->GetHeight();
		command->DepthTest = DepthTestingMode::Off;
		command->Blending = BlendingMode::Greatest;
		command->Culling = CullingMode::Off;

		command->UniformData
		.SetInput("u_Exposure", exposure);
		command->UniformData
		.SetInput("u_BloomStrength", bloomStrength);
		command->UniformData
		.SetInput("u_BloomTexture",
			TextureSlot{ mips->Get(AttachmentTarget::Color), 0 });
		command->UniformData
		.SetInput("u_SceneTexture",
			TextureSlot{ src->Get(AttachmentTarget::Color), 1 });

		auto& call = command->NewDrawCall();
		call.VertexCount = 6;
		call.Primitive = PrimitiveType::Triangle;
		call.Partition = PartitionType::Single;
	}
	Renderer::EndPass();

	Renderer::Flush();
	UIRenderer::EndFrame();
}

void Bloom::InitMips() {
	uint32_t windowWidth = Application::GetWindow()->GetWidth();
	uint32_t windowHeight = Application::GetWindow()->GetHeight();

	glm::vec2 mipSize((float)windowWidth, (float)windowHeight);
	glm::ivec2 mipIntSize(windowWidth, windowHeight);

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
	auto* command = Renderer::NewCommand();
	command->Clear = true;
	command->UniformData
	.SetInput("u_SrcResolution",
		glm::vec2{ src->GetWidth(), src->GetHeight() });
	command->UniformData
	.SetInput("u_SrcTexture",
		TextureSlot{ src->Get(AttachmentTarget::Color), 0 });

	uint32_t i = 0;
	for(const auto& mip : mipChain) {
		command->ViewportWidth = mip.IntSize.x;
		command->ViewportHeight = mip.IntSize.y;
		command->DepthTest = DepthTestingMode::Off;
		command->Blending = BlendingMode::Off;
		command->Culling = CullingMode::Off;
		command->Outputs = { { AttachmentTarget::Color, i++ } };

		auto& call = command->NewDrawCall();
		call.VertexCount = 6;
		call.Primitive = PrimitiveType::Triangle;
		call.Partition = PartitionType::Single;

		command = Renderer::NewCommand();
		command->UniformData
		.SetInput("u_SrcResolution", mip.Size);
		command->UniformData
		.SetInput("u_SrcTexture", TextureSlot{ mip.Sampler, 0 });
	}
}

void Bloom::Upsample() {
	auto* command = Renderer::NewCommand();
	command->UniformData
	.SetInput("u_FilterRadius", filterRadius);

	for(uint32_t i = mipChainLength - 1; i > 0; i--) {
		const BloomMip& mip = mipChain[i];
		const BloomMip& nextMip = mipChain[i - 1];

		command->ViewportWidth = nextMip.IntSize.x;
		command->ViewportHeight = nextMip.IntSize.y;
		command->DepthTest = DepthTestingMode::Off;
		command->Blending = BlendingMode::Additive;
		command->Culling = CullingMode::Off;
		command->Outputs = { { AttachmentTarget::Color, i - 1 } };
		command->UniformData
		.SetInput("u_SrcTexture", TextureSlot{ mip.Sampler, 0 });
		command->UniformData
		.SetInput("u_SrcResolution", mip.Size);

		auto& call = command->NewDrawCall();
		call.VertexCount = 6;
		call.Primitive = PrimitiveType::Triangle;
		call.Partition = PartitionType::Single;

		command = Renderer::NewCommand();
	}
}

}