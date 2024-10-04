#pragma once

#include <filesystem>

#include <VolcaniCore/Object/Texture.h>

#include "Scene/Scene.h"

namespace Magma {

class SceneHierarchyPanel {
public:
	SceneHierarchyPanel(Ref<Scene> scene);
	~SceneHierarchyPanel() = default;

	void Render();

	void SetContext(Ref<Scene> scene);

private:
	Ref<Scene> m_Context;
	ECS::Entity m_Selected;

private:
	template<typename T>
	void DisplayAddComponentEntry(const std::string& entryName);

	template<typename TComponent, typename TUIFunction>
	void DrawComponent(const std::string& name, ECS::Entity& entity,
					   TUIFunction uiFunction);

	void SetSelectedEntity(ECS::Entity& entity);
	void DrawEntityNode(ECS::Entity& entity);
	void DrawComponents(ECS::Entity& entity);
};

}