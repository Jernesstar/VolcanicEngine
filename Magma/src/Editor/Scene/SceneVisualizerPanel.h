#pragma once

#include "Panel.h"

#include "Scene/Scene.h"

#include "UI/Image.h"
#include "UI/Button.h"

namespace Magma {

struct EditorEntity {
	ECS::Entity Handle;
	Ref<Physics::RigidBody> Collider; // Used to press on Entity objects
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
	Ref<UI::Image> m_Image;

private:
	class Renderer : public SceneRenderer {
	public:
		Renderer();
		~Renderer() = default;
		void Update(TimeStep ts) override;
		void Submit(Entity entity) override;
		void Render() override;

		void Select(Entity entity) { Selected = entity; }

	private:
		Entity Selected;

		// Lighting
		Ref<RenderPass> Lighting;

		// Outlining
		Ref<RenderPass> Mask;
		Ref<RenderPass> Outline;

		// Shadows
		Ref<RenderPass> Depth;
		Ref<RenderPass> Shadow;

		// Bloom
		Ref<Framebuffer> Mips;
		Ref<RenderPass> Downsample;
		Ref<RenderPass> Upsample;
		Ref<RenderPass> Bloom;

		Ref<RenderPass> FinalComposite;
	};

private:
	Renderer m_Renderer;
};

}