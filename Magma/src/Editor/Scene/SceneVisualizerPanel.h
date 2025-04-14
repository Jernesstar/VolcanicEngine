#pragma once

#include <Magma/Scene/Scene.h>
#include <Magma/Scene/Component.h>

#include <Magma/Physics/World.h>

#include "Editor/Panel.h"
#include "Scene/SceneTab.h"

#include "UI/Image.h"
#include "UI/Button.h"

namespace Magma {

class SceneVisualizerPanel;

class EditorSceneRenderer : public SceneRenderer {
public:
	EditorSceneRenderer(SceneVisualizerPanel* panel);
	~EditorSceneRenderer();

	void Update(TimeStep ts) override;

	void Begin() override;
	void SubmitCamera(const Entity& entity) override;
	void SubmitSkybox(const Entity& entity) override;
	void SubmitLight(const Entity& entity) override;
	void SubmitParticles(const Entity& entity) override;
	void SubmitMesh(const Entity& entity) override;
	void Render() override;

	void Resize(uint32_t width, uint32_t height);
	void DrawRay(const glm::vec3& origin, const glm::vec3& end);
	void Select(Entity entity) { Selected = entity; }
	void IsHovered(bool hovered) { Hovered = hovered; }

private:
	SceneVisualizerPanel* Panel;
	glm::vec3 RayOrigin;
	glm::vec3 RayEnd;

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

	// Lighting and shadows
	Ref<RenderPass> DepthPass;
	Ref<RenderPass> LightingPass;
	Ref<UniformBuffer> DirectionalLightBuffer;
	Ref<UniformBuffer> PointLightBuffer;
	Ref<UniformBuffer> SpotlightBuffer;
	DrawCommand* LightingCommand;

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

	void Add(ECS::Entity entity);
	void Remove(ECS::Entity entity);
	void Select(ECS::Entity entity) {
		m_Selected = entity;
		m_Renderer.Select(m_Selected);
	}
	ECS::Entity GetSelected() { return m_Selected; }

	Physics::World& GetPhysicsWorld() { return m_World; }

private:
	Scene* m_Context;
	EditorSceneRenderer m_Renderer;
	Physics::World m_World;
	Entity m_Selected;
	UI::Image m_Image;

	friend class ProjectTab;
};

}