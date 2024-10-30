#pragma once

#include "Panel.h"
#include "Scene/Scene.h"

namespace Magma {

class SceneHierarchyPanel : public Panel {
public:
	SceneHierarchyPanel();
	SceneHierarchyPanel(Ref<Scene> scene);
	~SceneHierarchyPanel() = default;

	void SetContext(Ref<Scene> scene);

	void Update(TimeStep ts) override;
	void Draw() override;

	void Select(ECS::Entity entity) {
		m_Selected = entity;
	}
	ECS::Entity GetSelected() { return m_Selected; }

private:
	Ref<Scene> m_Context;
	ECS::Entity m_Selected;

private:
	void DrawEntityNode(ECS::Entity& entity);
};

}