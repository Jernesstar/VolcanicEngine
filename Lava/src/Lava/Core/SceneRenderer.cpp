#include "SceneRenderer.h"

#include <VolcaniCore/Core/Application.h>

#include <VolcaniCore/Graphics/Renderer.h>
#include <VolcaniCore/Graphics/RendererAPI.h>
#include <VolcaniCore/Graphics/Renderer2D.h>
#include <VolcaniCore/Graphics/Renderer3D.h>
#include <VolcaniCore/Graphics/StereographicCamera.h>
#include <VolcaniCore/Graphics/ShaderLibrary.h>

#include <Magma/Scene/Component.h>

#include "App.h"

namespace Lava {

struct DirectionalLight {
	glm::vec4 Position;
	glm::vec4 Ambient;
	glm::vec4 Diffuse;
	glm::vec4 Specular;
	glm::vec4 Direction;

	DirectionalLight(const DirectionalLightComponent& dc)
		: Position(dc.Position, 0.0f), Ambient(dc.Ambient, 0.0f),
		Diffuse(dc.Diffuse, 0.0f), Specular(dc.Specular, 0.0f),
		Direction(dc.Direction, 0.0f) { }
};

struct PointLight {
	glm::vec4 Position;
	glm::vec4 Ambient;
	glm::vec4 Diffuse;
	glm::vec4 Specular;

	float Constant;
	float Linear;
	float Quadratic;
	float BloomStrength;

	PointLight(const PointLightComponent& pc)
		: Position(pc.Position, 0.0f), Ambient(pc.Ambient, 0.0f),
		Diffuse(pc.Diffuse, 0.0f), Specular(pc.Specular, 0.0f),
		Constant(pc.Constant), Linear(pc.Linear), Quadratic(pc.Quadratic),
		BloomStrength(pc.Bloom ? 0.04f : 0.0f) { }
};

struct Spotlight {
	glm::vec4 Position;
	glm::vec4 Ambient;
	glm::vec4 Diffuse;
	glm::vec4 Specular;
	glm::vec4 Direction;

	float CutoffAngle;
	float OuterCutoffAngle;
	float _padding1;
	float _padding2;

	Spotlight(const SpotlightComponent& sc)
		: Position(sc.Position, 0.0f), Ambient(sc.Ambient, 0.0f),
		Diffuse(sc.Diffuse, 0.0f), Specular(sc.Specular, 0.0f),
		Direction(sc.Direction, 0.0f),
		CutoffAngle(sc.CutoffAngle), OuterCutoffAngle(sc.OuterCutoffAngle) { }
};

struct BloomMip {
	glm::vec2 Size;
	glm::ivec2 IntSize;
	Ref<Texture> Sampler;
};

struct ParticleData {
	Vec3 Position;
	Vec3 Velocity;
	float Life;
};

struct ParticleEmitter {
	Vec3 Position;
	uint64_t MaxParticleCount;
	float ParticleLifetime; // In milliseconds
	float SpawnInterval; // In milliseconds
	float Offset;
	float Timer;

	Ref<Texture> Material;
	Ref<StorageBuffer> ParticleBuffer;
	Ref<StorageBuffer> FreeListBuffer;
	Ref<RenderPass> DrawPass;
};

static List<BloomMip> s_MipChain;
static uint32_t s_MipChainLength = 10;
static float s_FilterRadius = 0.005f;
// TODO(Move): These are material settings
static float s_Exposure = 1.0f;
static float s_BloomStrength = 0.04f;

static Map<uint64_t, ParticleEmitter> s_ParticleEmitters;

RuntimeSceneRenderer::RuntimeSceneRenderer() {
	auto window = Application::GetWindow();
	m_Output = Framebuffer::Create(window->GetWidth(), window->GetHeight());

	DirectionalLightBuffer =
		UniformBuffer::Create(
			BufferLayout
			{
				{ "Position",  BufferDataType::Vec4 },
				{ "Ambient",   BufferDataType::Vec4 },
				{ "Diffuse",   BufferDataType::Vec4 },
				{ "Specular",  BufferDataType::Vec4 },
				{ "Direction", BufferDataType::Vec4 },
			}, 1);

	PointLightBuffer =
		UniformBuffer::Create(
			BufferLayout
			{
				{ "Position",  BufferDataType::Vec4 },
				{ "Ambient",   BufferDataType::Vec4 },
				{ "Diffuse",   BufferDataType::Vec4 },
				{ "Specular",  BufferDataType::Vec4 },
				{ "Constant",  BufferDataType::Float },
				{ "Linear",	   BufferDataType::Float },
				{ "Quadratic", BufferDataType::Float },
				{ "BloomStrength", BufferDataType::Float },
			}, 50);

	SpotlightBuffer =
		UniformBuffer::Create(
			BufferLayout
			{
				{ "Position",  BufferDataType::Vec4 },
				{ "Ambient",   BufferDataType::Vec4 },
				{ "Diffuse",   BufferDataType::Vec4 },
				{ "Specular",  BufferDataType::Vec4 },
				{ "Direction", BufferDataType::Vec4 },
				{ "CutoffAngle",	  BufferDataType::Float },
				{ "OuterCutoffAngle", BufferDataType::Float },
				{ "_padding1", BufferDataType::Float },
				{ "_padding2", BufferDataType::Float },
			}, 50);

	LightingPass =
		RenderPass::Create("Lighting",
			ShaderLibrary::Get("Lighting"), m_Output);
	LightingPass->SetData(Renderer3D::GetMeshBuffer());

	BaseLayer = Framebuffer::Create(window->GetWidth(), window->GetHeight());

	LightPass =
		RenderPass::Create("Light",
			ShaderLibrary::Get("Light"), BaseLayer);
	LightPass->SetData(Renderer2D::GetScreenBuffer());

	InitMips();
	DownsamplePass =
		RenderPass::Create("Bloom-Downsample",
			ShaderLibrary::Get("Bloom-Downsample"), Mips);
	UpsamplePass =
		RenderPass::Create("Bloom-Upsample",
			ShaderLibrary::Get("Bloom-Upsample"), Mips);
	BloomPass =
		RenderPass::Create("Bloom",
			ShaderLibrary::Get("Bloom"), m_Output);

	DownsamplePass->SetData(Renderer2D::GetScreenBuffer());
	UpsamplePass->SetData(Renderer2D::GetScreenBuffer());
	BloomPass->SetData(Renderer2D::GetScreenBuffer());

	EmitterPass =
		RenderPass::Create("Particle-Emit",
			ShaderLibrary::Get("Particle-Emit"));
	UpdatePass =
		RenderPass::Create("Particle-Update",
			ShaderLibrary::Get("Particle-Update"));
	ParticlePass =
		RenderPass::Create("Particle-Draw",
			ShaderLibrary::Get("Particle-DefaultDraw"), m_Output);
	ParticlePass->SetData(Renderer3D::GetMeshBuffer());
}

void RuntimeSceneRenderer::OnSceneLoad() {
	auto* scene = App::Get()->GetScene();

	BufferLayout particleLayout =
	{
		{ "Position", BufferDataType::Vec3 },
		{ "Velocity", BufferDataType::Vec3 },
		{ "Life", BufferDataType::Float },
	};
	BufferLayout freeListLayout =
	{
		{ "Indices", BufferDataType::Int },
	};

	scene->EntityWorld.GetNative()
	.observer<ParticleEmitterComponent>()
	.event(flecs::OnSet)
	.each(
		[=](flecs::entity e, ParticleEmitterComponent& component)
		{
			auto& emitter = s_ParticleEmitters[e];

			emitter.Position = component.Position;
			emitter.ParticleLifetime = component.ParticleLifetime;
			emitter.SpawnInterval = component.SpawnInterval;
			emitter.Offset = component.Offset;
			emitter.Timer = 0.0f;

			if(emitter.MaxParticleCount != component.MaxParticleCount) {
				emitter.MaxParticleCount = component.MaxParticleCount;

				Buffer<ParticleData> data(emitter.MaxParticleCount);
				for(uint32_t i = 0; i < emitter.MaxParticleCount; i++)
					data.Set(i, ParticleData{ });

				Buffer<int> freelist(emitter.MaxParticleCount + 1);
				freelist.Set(0, (int)emitter.MaxParticleCount);
				for(uint32_t i = 1; i <= emitter.MaxParticleCount; i++)
					freelist.Set(i, int(i) - 1);

				emitter.ParticleBuffer =
					StorageBuffer::Create(particleLayout, data);
				emitter.FreeListBuffer =
					StorageBuffer::Create(freeListLayout, freelist);
			}
		});
}

void RuntimeSceneRenderer::OnSceneClose() {
	s_ParticleEmitters.clear();
}

void RuntimeSceneRenderer::Update(TimeStep ts) {
	Renderer::StartPass(EmitterPass);
	{
		int workGroupSize = 64;
		auto* command = Renderer::GetCommand();
		command->UniformData
		.SetInput("u_TimeStep", (float)ts);

		for(auto& [_, emitter] : s_ParticleEmitters) {
			emitter.Timer += (float)ts;
			uint32_t particlesToSpawn = emitter.Timer / emitter.SpawnInterval;
			emitter.Timer = glm::mod(emitter.Timer, emitter.SpawnInterval);

			if(particlesToSpawn <= 0)
				continue;

			uint32_t numWorkGroups =
				(particlesToSpawn + workGroupSize - 1) / workGroupSize;

			auto* command = Renderer::NewCommand();
			command->ComputeX = numWorkGroups;
			command->UniformData
			.SetInput("u_ParticlesToSpawn", (int)particlesToSpawn);
			command->UniformData
			.SetInput("u_Emitter.Position", emitter.Position);
			command->UniformData
			.SetInput("u_Emitter.ParticleLifetime", emitter.ParticleLifetime);
			command->UniformData
			.SetInput("u_Emitter.Offset", emitter.Offset);
			command->UniformData
			.SetInput(StorageSlot{ emitter.ParticleBuffer, "", 0 });
			command->UniformData
			.SetInput(StorageSlot{ emitter.FreeListBuffer, "", 1 });
		}
	}
	Renderer::EndPass();

	Renderer::StartPass(UpdatePass);
	{
		int workGroupSize = 128;
		auto* command = Renderer::GetCommand();
		command->UniformData
		.SetInput("u_TimeStep", (float)ts);
		for(auto& [_, emitter] : s_ParticleEmitters) {
			uint32_t numWorkGroups =
				(emitter.MaxParticleCount + workGroupSize - 1) / workGroupSize;

			command = Renderer::NewCommand();
			command->ComputeX = numWorkGroups;
			command->UniformData
			.SetInput(StorageSlot{ emitter.ParticleBuffer, "", 0 });
			command->UniformData
			.SetInput(StorageSlot{ emitter.FreeListBuffer, "", 1 });
		}
	}
	Renderer::EndPass();
}

void RuntimeSceneRenderer::Begin() {
	LightCommand = RendererAPI::Get()->NewDrawCommand(LightPass->Get());
	LightCommand->Clear = true;
	LightCommand->DepthTest = DepthTestingMode::On;
	LightCommand->Blending = BlendingMode::Greatest;
	LightCommand->Culling = CullingMode::Off;

	Renderer::StartPass(DownsamplePass, false);
	{
		Downsample();
	}

	Renderer::StartPass(UpsamplePass, false);
	{
		Upsample();
	}
	Renderer::EndPass();

	Renderer::StartPass(BloomPass, false);
	{
		Composite();
	}

	LightingCommand = RendererAPI::Get()->NewDrawCommand(LightingPass->Get());
}

void RuntimeSceneRenderer::SubmitCamera(const Entity& entity) {
	auto camera = entity.Get<CameraComponent>().Cam;
	if(!camera)
		return;

	LightingCommand->UniformData
	.SetInput("u_View", camera->GetView());
	LightingCommand->UniformData
	.SetInput("u_ViewProj", camera->GetViewProjection());
	LightingCommand->UniformData
	.SetInput("u_CameraPosition", camera->GetPosition());
}

void RuntimeSceneRenderer::SubmitSkybox(const Entity& entity) {
	auto& sc = entity.Get<SkyboxComponent>();
	auto* assetManager = AssetManager::Get();
	assetManager->Load(sc.CubemapAsset);
	auto cubemap = assetManager->Get<Cubemap>(sc.CubemapAsset);

	LightingCommand->UniformData
	.SetInput("u_Skybox", CubemapSlot{ cubemap, 0 });
}

void RuntimeSceneRenderer::SubmitLight(const Entity& entity) {
	if(entity.Has<DirectionalLightComponent>()) {
		DirectionalLight light = entity.Get<DirectionalLightComponent>();
		DirectionalLightBuffer->SetData(&light);
		HasDirectionalLight = true;
	}
	else if(entity.Has<PointLightComponent>()) {
		PointLight light = entity.Get<PointLightComponent>();
		PointLightBuffer->SetData(&light, 1, PointLightCount++);
	}
	else if(entity.Has<SpotlightComponent>()) {
		Spotlight light = entity.Get<SpotlightComponent>();
		SpotlightBuffer->SetData(&light, 1, SpotlightCount++);
	}
}

void RuntimeSceneRenderer::SubmitParticles(const Entity& entity) {
	auto& emitter = s_ParticleEmitters[entity.GetHandle()];

	Renderer::StartPass(ParticlePass);
	{
		auto* command = Renderer::GetCommand();
		command->UniformData
		.SetInput("u_View",
			LightingCommand->UniformData.Mat4Uniforms["u_View"]);
		command->UniformData
		.SetInput("u_ViewProj",
			LightingCommand->UniformData.Mat4Uniforms["u_ViewProj"]);
		command->UniformData
		.SetInput("u_BillboardWidth", 0.1f);
		command->UniformData
		.SetInput("u_BillboardHeight", 0.1f);

		command->DepthTest = DepthTestingMode::On;
		command->Culling = CullingMode::Off;
		command->Blending = BlendingMode::Greatest;
		command->UniformData
		.SetInput("u_Texture", TextureSlot{ emitter.Material, 0 });

		auto& call = command->NewDrawCall();
		call.VertexCount = 6;
		call.InstanceCount = emitter.MaxParticleCount;
		call.Primitive = PrimitiveType::Triangle;
		call.Partition = PartitionType::Instanced;
	}
	Renderer::EndPass();
}

void RuntimeSceneRenderer::SubmitMesh(const Entity& entity) {
	auto& tc = entity.Get<TransformComponent>();
	auto& mc = entity.Get<MeshComponent>();
	auto* assetManager = AssetManager::Get();

	if(!assetManager->IsValid(mc.MeshSourceAsset))
		return;

	assetManager->Load(mc.MeshSourceAsset);
	auto mesh = assetManager->Get<Mesh>(mc.MeshSourceAsset);

	if(!mc.MaterialAsset.ID) {
		Renderer::StartPass(LightingPass);
		{
			Renderer3D::DrawMesh(mesh, tc);
		}
		Renderer::EndPass();
		return;
	}

	if(!assetManager->IsValid(mc.MaterialAsset))
		return;

	VolcaniCore::Material mat;
	assetManager->Load(mc.MaterialAsset);
	auto material = assetManager->Get<Magma::Material>(mc.MaterialAsset);

	if(material->TextureUniforms.count("u_Diffuse")) {
		UUID id = material->TextureUniforms["u_Diffuse"];
		Asset textureAsset = { id, AssetType::Texture };
		assetManager->Load(textureAsset);
		mat.Diffuse = assetManager->Get<Texture>(textureAsset);
	}

	if(material->TextureUniforms.count("u_Specular")) {
		UUID id = material->TextureUniforms["u_Specular"];
		Asset textureAsset = { id, AssetType::Texture };
		assetManager->Load(textureAsset);
		mat.Specular = assetManager->Get<Texture>(textureAsset);
	}

	if(material->TextureUniforms.count("u_Emissive")) {
		UUID id = material->TextureUniforms["u_Emissive"];
		Asset textureAsset = { id, AssetType::Texture };
		assetManager->Load(textureAsset);
		mat.Emissive = assetManager->Get<Texture>(textureAsset);
	}

	if(material->Vec4Uniforms.count("u_DiffuseColor"))
		mat.DiffuseColor = material->Vec4Uniforms["u_DiffuseColor"];
	if(material->Vec4Uniforms.count("u_SpecularColor"))
		mat.SpecularColor = material->Vec4Uniforms["u_SpecularColor"];
	if(material->Vec4Uniforms.count("u_EmissiveColor"))
		mat.EmissiveColor = material->Vec4Uniforms["u_EmissiveColor"];

	DrawCommand* command =
		RendererAPI::Get()->NewDrawCommand(LightingPass->Get());
	command->UniformData
	.SetInput("u_Material.IsTextured", (bool)mat.Diffuse);
	command->UniformData
	.SetInput("u_Material.Diffuse", TextureSlot{ mat.Diffuse, 0 });
	command->UniformData
	.SetInput("u_Material.Specular", TextureSlot{ mat.Specular, 1 });
	command->UniformData
	.SetInput("u_Material.Emissive", TextureSlot{ mat.Emissive, 2 });

	command->UniformData
	.SetInput("u_Material.DiffuseColor", mat.DiffuseColor);
	command->UniformData
	.SetInput("u_Material.SpecularColor", mat.SpecularColor);
	command->UniformData
	.SetInput("u_Material.EmissiveColor", mat.EmissiveColor);

	Renderer3D::DrawMesh(mesh, tc, command);
}

void RuntimeSceneRenderer::Render() {
	Renderer3D::End();

	LightingCommand->UniformData
	.SetInput("u_DirectionalLightCount", (int32_t)HasDirectionalLight);
	LightingCommand->UniformData
	.SetInput("u_PointLightCount", (int32_t)PointLightCount);
	LightingCommand->UniformData
	.SetInput("u_SpotlightCount", (int32_t)SpotlightCount);

	LightingCommand->UniformData
	.SetInput(UniformSlot{ DirectionalLightBuffer, "", 0 });
	LightingCommand->UniformData
	.SetInput(UniformSlot{ PointLightBuffer, "", 1 });
	LightingCommand->UniformData
	.SetInput(UniformSlot{ SpotlightBuffer, "", 2 });

	LightCommand->UniformData
	.SetInput("u_View",
		LightingCommand->UniformData.Mat4Uniforms["u_View"]);
	LightCommand->UniformData
	.SetInput("u_ViewProj",
		LightingCommand->UniformData.Mat4Uniforms["u_ViewProj"]);
	LightCommand->UniformData
	.SetInput("u_Radius", 1.0f);
	LightCommand->UniformData
	.SetInput("u_CameraPosition",
		LightingCommand->UniformData.Vec3Uniforms["u_CameraPosition"]);
	LightCommand->UniformData
	.SetInput("u_ViewportWidth", (float)Application::GetWindow()->GetWidth());
	LightCommand->UniformData
	.SetInput("u_ViewportHeight", (float)Application::GetWindow()->GetHeight());
	LightCommand->UniformData
	.SetInput(UniformSlot{ PointLightBuffer, "", 1 });

	auto& call = LightCommand->NewDrawCall();
	call.VertexCount = 6;
	call.InstanceStart = 0;
	call.InstanceCount = PointLightCount;
	call.Primitive = PrimitiveType::Triangle;
	call.Partition = PartitionType::Instanced;

	Renderer::Flush();

	HasDirectionalLight = false;
	PointLightCount = 0;
	SpotlightCount = 0;
}

void RuntimeSceneRenderer::InitMips() {
	auto window = Application::GetWindow();

	glm::vec2 mipSize((float)window->GetWidth(), (float)window->GetHeight());
	glm::ivec2 mipIntSize(window->GetWidth(), window->GetHeight());
	List<Ref<Texture>> textures;
	for(uint32_t i = 0; i < s_MipChainLength; i++) {
		BloomMip mip;

		mipSize *= 0.5f;
		mipIntSize /= 2;
		mip.Size = mipSize;
		mip.IntSize = mipIntSize;

		mip.Sampler =
			Texture::Create(mipIntSize.x, mipIntSize.y, Texture::Format::Float);

		s_MipChain.Add(mip);
		textures.Add(mip.Sampler);
	}

	Mips = Framebuffer::Create(
		{
			{ AttachmentTarget::Color, textures }
		});
}

void RuntimeSceneRenderer::Downsample() {
	auto* command = Renderer::NewCommand();
	command->Clear = true;
	command->UniformData
	.SetInput("u_SrcResolution",
		glm::vec2{ BaseLayer->GetWidth(), BaseLayer->GetHeight() });
	command->UniformData
	.SetInput("u_SrcTexture",
		TextureSlot{ BaseLayer->Get(AttachmentTarget::Color), 0 });

	uint32_t i = 0;
	for(const auto& mip : s_MipChain) {
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

void RuntimeSceneRenderer::Upsample() {
	auto* command = Renderer::NewCommand();
	command->UniformData
	.SetInput("u_FilterRadius", s_FilterRadius);

	for(uint32_t i = s_MipChainLength - 1; i > 0; i--) {
		const BloomMip& mip = s_MipChain[i];
		const BloomMip& nextMip = s_MipChain[i - 1];

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

void RuntimeSceneRenderer::Composite() {
	auto* command = Renderer::NewCommand();
	command->Clear = true;
	command->ViewportWidth = Application::GetWindow()->GetWidth();
	command->ViewportHeight = Application::GetWindow()->GetHeight();
	command->DepthTest = DepthTestingMode::Off;
	command->Blending = BlendingMode::Greatest;
	command->Culling = CullingMode::Off;

	command->UniformData
	.SetInput("u_Exposure", s_Exposure);
	command->UniformData
	.SetInput("u_BloomStrength", s_BloomStrength);
	command->UniformData
	.SetInput("u_BloomTexture",
		TextureSlot{ Mips->Get(AttachmentTarget::Color), 0 });
	command->UniformData
	.SetInput("u_SceneTexture",
		TextureSlot{ BaseLayer->Get(AttachmentTarget::Color), 1 });

	auto& call = command->NewDrawCall();
	call.VertexCount = 6;
	call.Primitive = PrimitiveType::Triangle;
	call.Partition = PartitionType::Single;
}

}