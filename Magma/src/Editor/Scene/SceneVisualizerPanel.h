#pragma once

#include <Magma/Scene/Scene.h>
#include <Magma/Scene/Component.h>

#include <Magma/Physics/World.h>

#include "Editor/Panel.h"
#include "Scene/SceneTab.h"

#include "UI/Image.h"
#include "UI/Button.h"

namespace Magma {

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

	void Select(Entity entity) { Selected = entity; }
	void IsHovered(bool hovered) { Hovered = hovered; }

private:
	Entity Selected;
	bool Hovered = false;

	// Grid
	Ref<RenderPass> GridPass;

	// Outlining
	Ref<RenderPass> MaskPass;
	Ref<RenderPass> OutlinePass;

	// Billboards
	Ref<RenderPass> BillboardPass;
	DrawBuffer* BillboardBuffer;
	DrawCommand* DirectionalLightBillboardCommand;
	DrawCommand* PointLightBillboardCommand;
	DrawCommand* SpotlightBillboardCommand;
	Ref<Texture> DirectionalLightIcon;
	Ref<Texture> PointLightIcon;
	Ref<Texture> SpotlightIcon;

	// Lighting and shadows
	Ref<RenderPass> DepthPass;
	Ref<RenderPass> LightingPass;
	Ref<UniformBuffer> DirectionalLightBuffer;
	Ref<UniformBuffer> PointLightBuffer;
	Ref<UniformBuffer> SpotlightBuffer;
	DrawCommand* FirstCommand;

	bool HasDirectionalLight = false;
	uint32_t PointLightCount = 0;
	uint32_t SpotlightCount = 0;

	// Bloom
	Ref<Framebuffer> Mips;
	Ref<RenderPass> DownsamplePass;
	Ref<RenderPass> UpsamplePass;
	Ref<RenderPass> BloomPass;
};

class SceneVisualizerPanel : public Panel {
public:
	SceneVisualizerPanel(Scene* scene);
	~SceneVisualizerPanel() = default;

	void SetContext(Scene* scene);

	void Update(TimeStep ts) override;
	void Draw() override;

	void Select(ECS::Entity entity) {
		m_Selected = entity;
	}
	ECS::Entity GetSelected() { return m_Selected; }

private:
	Scene* m_Context;
	Physics::World m_World;
	Entity m_Selected;
	UI::Image m_Image;

private:
	EditorSceneRenderer m_Renderer;
};

}