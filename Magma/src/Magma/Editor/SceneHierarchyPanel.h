#pragma once

#include <filesystem>

#include <VolcaniCore/Renderer/Texture.h>

#include "Scene.h"

namespace Magma {

class SceneHierarchyPanel {
public:
	SceneHierarchyPanel(const Ref<Scene>& scene);
	~SceneHierarchyPanel() = default;

	void Render();

private:
	Ref<Scene> m_Context;
	Entity* m_SelectionContext;
	std::filesystem::path m_BaseDirectory;
	std::filesystem::path m_CurrentDirectory;

private:
	template<typename T>
	void DisplayAddComponentEntry(const std::string& entryName);

	template<typename TComponent, typename UIFunction>
	void DrawComponent(const std::string& name, Entity& entity, UIFunction uiFunction);

	void SetContext(const Ref<Scene>& scene);
	void SetSelectedEntity(Entity& entity);
	void DrawEntityNode(Entity& entity);
	void DrawComponents(Entity& entity);
};

}