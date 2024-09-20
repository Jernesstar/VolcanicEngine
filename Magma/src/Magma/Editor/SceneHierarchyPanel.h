#pragma once

#include <filesystem>

#include <VolcaniCore/Object/Texture.h>

#include "Scene/Scene.h"

namespace Magma {

class SceneHierarchyPanel {
public:
	SceneHierarchyPanel(const Ref<Scene>& scene);
	~SceneHierarchyPanel() = default;

	void Render();

private:
	Ref<Scene> m_Context;
	ECS::Entity* m_SelectionContext;
	std::filesystem::path m_BaseDirectory;
	std::filesystem::path m_CurrentDirectory;

private:
	template<typename T>
	void DisplayAddComponentEntry(const std::string& entryName);

	template<typename TComponent, typename TUIFunction>
	void DrawComponent(const std::string& name, ECS::Entity& entity,
					   TUIFunction uiFunction);

	void SetContext(const Ref<Scene>& scene);
	void SetSelectedEntity(ECS::Entity& entity);
	void DrawEntityNode(ECS::Entity& entity);
	void DrawComponents(ECS::Entity& entity);
};

}