#pragma once

#include <VolcaniCore/Core/Defines.h>

#include <Magma/Scene/Scene.h>
#include <Magma/Scene/SceneRenderer.h>

using namespace VolcaniCore;
using namespace Magma;

namespace Lava {

class RuntimeSceneRenderer : public SceneRenderer {
public:
	RuntimeSceneRenderer();
	~RuntimeSceneRenderer() = default;

	void Update(TimeStep ts) override;

	void Begin() override;
	void SubmitCamera(const Entity& entity) override;
	void SubmitSkybox(const Entity& entity) override;
	void SubmitLight(const Entity& entity) override;
	void SubmitParticles(const Entity& entity) override;
	void SubmitMesh(const Entity& entity) override;
	void Render() override;

	void OnSceneLoad();
	void OnSceneClose();

private:
	// Base
	Ref<Framebuffer> m_BaseLayer;

	// End
	Ref<RenderPass> FinalCompositePass;

	// Lighting and shadows
	Ref<RenderPass> DepthPass;
	Ref<RenderPass> LightingPass;
	DrawCommand* LightingCommand;

	// Lights
	Ref<RenderPass> LightPass;
	DrawCommand* LightCommand;

	Ref<UniformBuffer> DirectionalLightBuffer;
	Ref<UniformBuffer> PointLightBuffer;
	Ref<UniformBuffer> SpotlightBuffer;
	bool HasDirectionalLight = false;
	uint32_t PointLightCount = 0;
	uint32_t SpotlightCount = 0;

	// Bloom
	Ref<Framebuffer> Mips;
	Ref<RenderPass> DownsamplePass;
	Ref<RenderPass> UpsamplePass;
	Ref<RenderPass> BloomPass;

	// Particles
	Ref<RenderPass> EmitterPass;
	Ref<RenderPass> UpdatePass;
	Ref<RenderPass> ParticlePass;

private:
	void InitMips();
	void Downsample();
	void Upsample();
	void Composite();
};

}