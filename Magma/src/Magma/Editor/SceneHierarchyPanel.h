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

private:
	Ref<Scene> m_Context;
	ECS::Entity m_Selected;

private:
	void Draw() override;

	void DrawEntityNode(ECS::Entity& entity);
};

}