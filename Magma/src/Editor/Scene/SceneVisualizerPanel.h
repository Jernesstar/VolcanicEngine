#pragma once

#include <Magma/Scene/Scene.h>
#include <Magma/Scene/Component.h>

#include <Magma/Physics/World.h>

#include "Editor/Editor.h"
#include "Editor/Panel.h"
#include "Scene/SceneTab.h"

#include "UI/Image.h"
#include "UI/Button.h"

namespace Magma {

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
		Editor::GetSceneRenderer().Select(m_Selected);
	}
	ECS::Entity GetSelected() { return m_Selected; }

	Physics::World& GetPhysicsWorld() { return m_World; }

private:
	Scene* m_Context;
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