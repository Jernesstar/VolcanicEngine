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

class SceneVisualizerPanel : public Panel {
public:
	SceneVisualizerPanel(Scene* scene);
	~SceneVisualizerPanel() = default;

	void SetContext(Scene* scene);
	void SetImage();
	void ResetImage();

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
	UI::Button m_Cursor;
	UI::Button m_GizmoTranslate;
	UI::Button m_GizmoRotate;
	UI::Button m_GizmoScale;
	uint32_t m_GizmoMode = 0;

	friend class ProjectTab;
};

}