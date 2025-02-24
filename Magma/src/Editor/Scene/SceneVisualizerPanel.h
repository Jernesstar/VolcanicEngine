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
	~EditorSceneRenderer() = default;

	void Update(TimeStep ts) override;

	void Begin() override;
	void SubmitCamera(Entity entity) override;
	void SubmitSkybox(Entity entity) override;
	void SubmitLight(Entity entity) override;
	void SubmitMesh(Entity entity) override;
	void Render() override;

	void Select(Entity entity) { Selected = entity; }
	void SetState(SceneState state) { State = state; }
	void IsHovered(bool hovered) { Hovered = hovered; }

private:
	Entity Selected;
	SceneState State;
	bool Hovered = false;

	DrawCommand* FirstCommand;
	Map<Ref<Mesh>, DrawCommand*> Objects;

	// Lighting and shadows
	Ref<RenderPass> DepthPass;
	Ref<RenderPass> LightingPass;
	Ref<UniformBuffer> DirectionalLightBuffer;
	Ref<UniformBuffer> PointLightBuffer;
	Ref<UniformBuffer> SpotlightBuffer;
	bool HasDirectionalLight = false;
	uint32_t PointLightCount = 0;
	uint32_t SpotlightCount = 0;

	// Outlining
	Ref<RenderPass> MaskPass;
	Ref<RenderPass> OutlinePass;

	// Bloom
	Ref<Framebuffer> Mips;
	Ref<RenderPass> DownsamplePass;
	Ref<RenderPass> UpsamplePass;
	Ref<RenderPass> BloomPass;
};

struct EditorEntity {
	ECS::Entity Handle;
	Physics::RigidBody* Collider;
};

class SceneVisualizerPanel : public Panel {
public:
	SceneVisualizerPanel(Scene* scene);
	~SceneVisualizerPanel() = default;

	void SetContext(Scene* scene);

	void Update(TimeStep ts) override;
	void Draw() override;

	void Select(ECS::Entity entity) {
		m_Selected.Handle = entity;
	}
	ECS::Entity GetSelected() { return m_Selected.Handle; }

private:
	Scene* m_Context;
	EditorEntity m_Selected;
	Physics::World m_World;
	UI::Image m_Image;

private:
	EditorSceneRenderer m_Renderer;
};

}