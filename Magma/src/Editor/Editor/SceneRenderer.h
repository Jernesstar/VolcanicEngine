#pragma once

#include <Magma/Scene/Scene.h>
#include <Magma/Scene/Component.h>

namespace Magma {

class SceneVisualizerPanel;

class EditorSceneRenderer : public SceneRenderer {
public:
	EditorSceneRenderer();
	~EditorSceneRenderer();

	void Update(TimeStep ts) override;

	void Begin() override;
	void SubmitCamera(const Entity& entity) override;
	void SubmitSkybox(const Entity& entity) override;
	void SubmitLight(const Entity& entity) override;
	void SubmitParticles(const Entity& entity) override;
	void SubmitMesh(const Entity& entity) override;
	void Render() override;

	void SetContext(SceneVisualizerPanel* panel);
	CameraController& GetCameraController() { return m_Controller; }
	void Select(Entity entity) { Selected = entity; }
	void IsHovered(bool hovered) { Hovered = hovered; }

private:
	CameraController m_Controller;
	SceneVisualizerPanel* RootPanel;

	Entity Selected;
	bool Hovered = false;

	// Grid
	Ref<RenderPass> GridPass;

	// Outlining
	Ref<RenderPass> MaskPass;
	Ref<RenderPass> OutlinePass;

	// Lines
	Ref<RenderPass> LinePass;
	DrawCommand* LineCommand;

	// Billboards
	Ref<RenderPass> BillboardPass;
	DrawBuffer* BillboardBuffer;
	Ref<Texture> DirectionalLightIcon;
	Ref<Texture> PointLightIcon;
	Ref<Texture> SpotlightIcon;
	Ref<Texture> CameraIcon;
	Ref<Texture> ParticlesIcon;

	Ref<RenderPass> MeshPass;
	DrawCommand* MeshCommand;

	bool HasCamera = false;
	bool HasDirectionalLight = false;
	uint32_t PointLightCount = 0;
	uint32_t SpotlightCount = 0;
	uint32_t ParticleEmitterCount = 0;
	List<std::pair<glm::vec3, uint32_t>> Billboards;

private:
	void AddBillboard(const glm::vec3& position, uint32_t type);
};

}