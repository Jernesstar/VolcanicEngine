#pragma once

namespace Demo {

struct Emitter {
	Vec3 Position;
	Ref<Texture> Material;
	uint64_t MaxParticleCount;
	float ParticleLifetime; // In milliseconds
	float SpawnInterval; // In milliseconds
	float Timer;

	Ref<StorageBuffer> ParticleBuffer;
	Ref<StorageBuffer> FreeListBuffer;
};

struct ParticleData {
	Vec3 Position;
	Vec3 Velocity;
	float Life;
};

class Particles : public Application {
public:
	Particles();

	void OnUpdate(TimeStep ts) override;

private:
	Ref<Camera> camera;
	CameraController controller;

	Ref<RenderPass> EmitterPass;
	Ref<RenderPass> UpdatePass;
	Ref<RenderPass> DrawPass;

	List<Emitter> Emitters;
};

Particles::Particles() {
	Events::RegisterListener<KeyPressedEvent>(
		[](const KeyPressedEvent& event)
		{
			if(event.Key == Key::Escape)
				Application::Close();
		});
	Events::RegisterListener<KeyPressedEvent>(
		[](const KeyPressedEvent& event)
		{
			if(event.Key == Key::K)
				VOLCANICORE_LOG_INFO("Test Key K");
		});

	camera = CreateRef<StereographicCamera>(75.0f);
	camera->Resize(800, 600);
	camera->SetPosition({ 0.0f, 0.0f, 3.0f });
	camera->SetProjection(0.001f, 10'000.0f);
	controller = CameraController{ camera };

	VOLCANICORE_LOG_INFO("Particles Project Started");

	Ref<ShaderPipeline> shader;
	shader = AssetImporter::GetShader(
		{
			{ "Magma/assets/shaders/ParticleEmitter.glsl.comp", ShaderType::Compute },
		});
	EmitterPass = RenderPass::Create("Particle-Emit", shader);

	shader = AssetImporter::GetShader(
		{
			{ "Magma/assets/shaders/ParticleUpdate.glsl.comp", ShaderType::Compute },
		});
	UpdatePass = RenderPass::Create("Particle-Update", shader);

	shader = AssetImporter::GetShader(
		{
			{ "Magma/assets/shaders/Particle.glsl.vert", ShaderType::Vertex },
			{ "Magma/assets/shaders/Particle.glsl.frag", ShaderType::Fragment }
		});
	DrawPass = RenderPass::Create("Particle-Draw", shader);
	DrawPass->SetData(Renderer3D::GetMeshBuffer());

	auto texture = AssetImporter::GetTexture("Sandbox/assets/images/wood.png");
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

	Buffer<ParticleData> data(1000);
	for(uint32_t i = 0; i < 1000; i++)
		data.Set(i, ParticleData{ });

	Buffer<int> freelist(1001);
	freelist.Set(0, 1000);
	for(uint32_t i = 1; i <= 1000; i++)
		freelist.Set(i, int(i) - 1);

	Emitters.Add(
		{
			.Position = Vec3(0.0f),
			.Material = texture,
			.MaxParticleCount = 1000,
			.ParticleLifetime = 3000.0f,
			.SpawnInterval = 10.0f,
			.Timer = 0,
			.ParticleBuffer = StorageBuffer::Create(particleLayout, data),
			.FreeListBuffer = StorageBuffer::Create(freeListLayout, freelist)
		}
	);

	VOLCANICORE_LOG_INFO("Success");
}

void Particles::OnUpdate(TimeStep ts) {
	controller.OnUpdate(ts);

	Renderer::StartPass(EmitterPass);
	{
		int workGroupSize = 64;
		auto* command = Renderer::GetCommand();

		for(auto& emitter : Emitters) {
			emitter.Timer += (float)ts;
			uint32_t particlesToSpawn = emitter.Timer / emitter.SpawnInterval;
			emitter.Timer = glm::mod(emitter.Timer, emitter.SpawnInterval);

			if(particlesToSpawn <= 0)
				continue;

			command = Renderer::NewCommand();
			uint32_t numWorkGroups =
				(particlesToSpawn + workGroupSize - 1) / workGroupSize;
			command->ComputeX = numWorkGroups;
			command->UniformData
			.SetInput("u_TimeStep", (float)ts);
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
		int workGroupSize = 128;
		auto* command = Renderer::GetCommand();
		command->UniformData
		.SetInput("u_TimeStep", (float)ts);

		for(auto& emitter : Emitters) {
			command = Renderer::NewCommand();
			uint32_t numWorkGroups =
				(emitter.MaxParticleCount + workGroupSize - 1) / workGroupSize;
			command->ComputeX = numWorkGroups;
			command->UniformData
			.SetInput(StorageSlot{ emitter.ParticleBuffer, "", 0 });
			command->UniformData
			.SetInput(StorageSlot{ emitter.FreeListBuffer, "", 1 });
		}
	}
	Renderer::EndPass();

	Renderer::StartPass(DrawPass);
	{
		auto* command = Renderer::GetCommand();
		command->Clear = true;
		command->UniformData
		.SetInput("u_View", camera->GetView());
		command->UniformData
		.SetInput("u_ViewProj", camera->GetViewProjection());
		command->UniformData
		.SetInput("u_BillboardWidth", 0.1f);
		command->UniformData
		.SetInput("u_BillboardHeight", 0.1f);

		for(auto& emitter : Emitters) {
			command = Renderer::NewCommand();
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
	}
	Renderer::EndPass();
}

}