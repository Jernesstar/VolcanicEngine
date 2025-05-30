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

	PointLight(const PointLightComponent& pc)
		: Position(pc.Position, 0.0f), Ambient(pc.Ambient, 0.0f),
		Diffuse(pc.Diffuse, 0.0f), Specular(pc.Specular, 0.0f),
		Constant(pc.Constant), Linear(pc.Linear), Quadratic(pc.Quadratic) { }
};

struct Spotlight {
	glm::vec4 Position;
	glm::vec4 Ambient;
	glm::vec4 Diffuse;
	glm::vec4 Specular;
	glm::vec4 Direction;

	float CutoffAngle;
	float OuterCutoffAngle;

	Spotlight(const SpotlightComponent& sc)
		: Position(sc.Position, 0.0f), Ambient(sc.Ambient, 0.0f),
		Diffuse(sc.Diffuse, 0.0f), Specular(sc.Specular, 0.0f),
		Direction(sc.Direction, 0.0f),
		CutoffAngle(sc.CutoffAngle), OuterCutoffAngle(sc.OuterCutoffAngle) { }
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

	float Timer;

	Ref<Texture> Material;
	Ref<StorageBuffer> ParticleBuffer;
	Ref<StorageBuffer> FreeListBuffer;
	Ref<RenderPass> DrawPass;
};

static Map<uint64_t, ParticleEmitter> s_ParticleEmitters;

RuntimeSceneRenderer::RuntimeSceneRenderer() {
	auto window = Application::GetWindow();
	m_Output = Framebuffer::Create(window->GetWidth(), window->GetHeight());

	Ref<ShaderPipeline> shader;
	Ref<Framebuffer> buffer;

	shader = ShaderLibrary::Get("Lighting");
	// buffer = Framebuffer::Create(window->GetWidth(), window->GetHeight());
	LightingPass = RenderPass::Create("Lighting", shader, m_Output);
	LightingPass->SetData(Renderer3D::GetMeshBuffer());

	// shader = ShaderLibrary::Get("Bloom");
	// buffer = Framebuffer::Create(window->GetWidth(), window->GetHeight());
	// BloomPass = RenderPass::Create("Bloom", shader, buffer);
	// BloomPass->SetData(Renderer3D::GetMeshBuffer());

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
			}, 50);

	shader = ShaderLibrary::Get("Particle-Emit");
	EmitterPass = RenderPass::Create("Particle-Emit", shader);

	shader = ShaderLibrary::Get("Particle-Update");
	UpdatePass = RenderPass::Create("Particle-Update", shader);

	shader = ShaderLibrary::Get("Particle-DefaultDraw");
	DrawPass = RenderPass::Create("Particle-Draw", shader);
	DrawPass->SetData(Renderer3D::GetMeshBuffer());
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
	scene->EntityWorld.ForEach<ParticleEmitterComponent>(
		[&](Entity& entity)
		{
			auto& component = entity.Get<ParticleEmitterComponent>();
			auto& emitter = s_ParticleEmitters[entity.GetHandle()];

			emitter.Position = component.Position;
			emitter.MaxParticleCount = component.MaxParticleCount;
			emitter.SpawnInterval = component.SpawnInterval;
	
			Buffer<ParticleData> data(emitter.MaxParticleCount);
			for(uint32_t i = 0; i < emitter.MaxParticleCount; i++)
				data.Set(i, ParticleData{ });

			Buffer<int> freelist(emitter.MaxParticleCount + 1);
			freelist.Set(0, emitter.MaxParticleCount);
			for(uint32_t i = 1; i <= emitter.MaxParticleCount; i++)
				freelist.Set(i, int(i) - 1);

			emitter.ParticleBuffer =
				StorageBuffer::Create(particleLayout, data);
			emitter.FreeListBuffer =
				StorageBuffer::Create(freeListLayout, freelist);
		});
}

void RuntimeSceneRenderer::OnSceneClose() {

}

void RuntimeSceneRenderer::Update(TimeStep ts) {
	Renderer::StartPass(EmitterPass);
	{
		auto* command = Renderer::GetCommand();
		command->UniformData
		.SetInput("u_TimeStep", (float)ts);

		for(auto& [_, emitter] : s_ParticleEmitters) {
			emitter.Timer += (float)ts;
			uint32_t particlesToSpawn = emitter.Timer / emitter.SpawnInterval;
			emitter.Timer = glm::mod(emitter.Timer, emitter.SpawnInterval);
			if(particlesToSpawn <= 0)
				continue;

			int workGroupSize = 64;
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
			.SetInput(StorageSlot{ emitter.ParticleBuffer, "", 0 });
			command->UniformData
			.SetInput(StorageSlot{ emitter.FreeListBuffer, "", 1 });
		}
	}
	Renderer::EndPass();

	Renderer::StartPass(UpdatePass);
	{
		for(auto& [_, emitter] : s_ParticleEmitters) {
			int workGroupSize = 128;
			uint32_t numWorkGroups =
				(emitter.MaxParticleCount + workGroupSize - 1) / workGroupSize;

			auto* command = Renderer::NewCommand();
			command->ComputeX = numWorkGroups;
			command->UniformData
			.SetInput("u_TimeStep", (float)ts);
			command->UniformData
			.SetInput(StorageSlot{ emitter.ParticleBuffer, "", 0 });
			command->UniformData
			.SetInput(StorageSlot{ emitter.FreeListBuffer, "", 1 });
		}
	}
	Renderer::EndPass();
}

void RuntimeSceneRenderer::Begin() {
	LightingCommand = RendererAPI::Get()->NewDrawCommand(LightingPass->Get());
	LightingCommand->Clear = true;
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

	Renderer::StartPass(DrawPass);
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

	if(!assetManager->IsValid(mc.MeshAsset))
		return;

	assetManager->Load(mc.MeshAsset);
	auto mesh = assetManager->Get<Mesh>(mc.MeshAsset);

	Renderer::StartPass(LightingPass, false);
	{
		Renderer3D::DrawMesh(mesh, tc);
	}
	Renderer::EndPass();
}

void RuntimeSceneRenderer::Render() {
	LightingCommand->UniformData
	.SetInput("u_DirectionalLightCount", (int32_t)HasDirectionalLight);
	LightingCommand->UniformData
	.SetInput("u_PointLightCount", (int32_t)PointLightCount);
	LightingCommand->UniformData
	.SetInput("u_SpotlightCount", (int32_t)SpotlightCount);
	LightingCommand->UniformData
	.SetInput("u_SceneVisualizer", 0);

	LightingCommand->UniformData
	.SetInput(UniformSlot{ DirectionalLightBuffer, "", 0 });
	LightingCommand->UniformData
	.SetInput(UniformSlot{ PointLightBuffer, "", 1 });
	LightingCommand->UniformData
	.SetInput(UniformSlot{ SpotlightBuffer, "", 2 });

	Renderer3D::End();
	Renderer::Flush();

	HasDirectionalLight = false;
	PointLightCount = 0;
	SpotlightCount = 0;
}

}