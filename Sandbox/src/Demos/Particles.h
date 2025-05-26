#pragma once

namespace Demo {

struct Emitter {
	Vec3 Position;
	Ref<Texture> Material;
	uint64_t MaxParticleCount;
	float ParticleLifetime; // In milliseconds
	float SpawnInterval; // In milliseconds
	float Timer;
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
	camera->SetPosition({ 0.0f, 0.0f, 3.0f });
	controller = CameraController{ camera };

	VOLCANICORE_LOG_INFO("Particles Project Started");

	Ref<ShaderPipeline> shader;
	shader = ShaderPipeline::Create("Magma/assets/shaders", "ParticleEmitter");
	EmitterPass = RenderPass::Create("Particle-Emit", shader);

	shader = ShaderPipeline::Create("Magma/assets/shaders", "ParticleUpdate");
	UpdatePass = RenderPass::Create("Particle-Update", shader);

	shader = ShaderPipeline::Create("Magma/assets/shaders", "Particle");
	DrawPass = RenderPass::Create("Particle-Draw", shader);

	VOLCANICORE_LOG_INFO("Success");
}

void Particles::OnUpdate(TimeStep ts) {
	controller.OnUpdate(ts);
	int workGroupSize = 128;

	Renderer::StartPass(EmitterPass);
	{
		auto* command = Renderer::GetCommand();

		for(auto& emitter : Emitters) {
			command = Renderer::NewCommand();
			emitter.Timer += (float)ts;
			uint32_t particlesToSpawn = emitter.Timer / emitter.SpawnInterval;
			emitter.Timer = fmodf(emitter.Timer, emitter.SpawnInterval);
			if(particlesToSpawn <= 0)
				continue;

			int numWorkGroups =
				(emitter.MaxParticleCount + workGroupSize - 1) / workGroupSize;
			// command->UniformData
			// .SetInput(StorageSlot{ emitter.ParticleBuffer, "", 0 });
			// command->UniformData
			// .SetInput(StorageSlot{ emitter.FreeListBuffer, "", 1 });
			command->ComputeX = numWorkGroups;
		}
	}
	Renderer::EndPass();

	Renderer::StartPass(UpdatePass);
	{
		auto* command = Renderer::GetCommand();
		command->UniformData
		.SetInput("u_TimeStep", (float)ts);
		for(auto& emitter : Emitters) {
			command = Renderer::NewCommand();
			int numWorkGroups =
				(emitter.MaxParticleCount + workGroupSize - 1) / workGroupSize;
			// command->UniformData
			// .SetInput(StorageSlot{ ParticleBuffer, "", 0 });
			// command->UniformData
			// .SetInput(StorageSlot{ FreeListBuffer, "", 1 });
			command->ComputeX = numWorkGroups;
		}
	}
	Renderer::EndPass();

	Renderer::StartPass(DrawPass);
	{
		auto* command = Renderer::GetCommand();
		command->UniformData
		.SetInput("u_View", camera->GetView());
		command->UniformData
		.SetInput("u_ViewProj", camera->GetViewProjection());

		for(auto& emitter : Emitters) {
			command = Renderer::NewCommand();
			command->UniformData
			.SetInput("u_Texture", TextureSlot{ emitter.Material, 0 });

			auto& call = command->NewDrawCall();
			call.VertexCount = 6;
			call.InstanceCount = 100; // MaxParticleCount
			call.Primitive = PrimitiveType::Triangle;
			call.Partition = PartitionType::Instanced;
		}
	}
	Renderer::EndPass();
}

}