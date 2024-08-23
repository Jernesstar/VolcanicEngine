#pragma once

#include <filesystem>

#include <VolcaniCore/Core/Defines.h>

#include <VolcaniCore/Core/Time.h>
#include <VolcaniCore/Object/Texture.h>
#include <VolcaniCore/Object/Framebuffer.h>

#include <VolcaniCore/Renderer/Camera.h>

// #include "Scene.h"
// #include "SceneHierarchyPanel.h"

using namespace VolcaniCore;

namespace Magma {

class EditorLayer {
public:
	EditorLayer();
	~EditorLayer();

	void Update(VolcaniCore::TimeStep ts);
	void Render();

private:
	// Ref<Scene> m_CurrentScene;
	// Ref<SceneHierarchyPanel> m_SceneHierarchyPanel;
	Ref<Framebuffer> m_Framebuffer;
	Ref<Texture> m_IconPlay, m_IconPause, m_IconStop;
	
	enum class SceneState { Edit, Play, Pause };
	SceneState m_SceneState = SceneState::Edit;

	bool m_ViewportFocused;
	bool m_ViewportHovered;
	glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
	glm::vec2 m_ViewportBounds[2];

	void NewScene();
	void OpenScene();
	void OpenScene(const std::filesystem::path& path);
	void SaveScene();

	void AddEntity();

	void UI_Toolbar();
	void OpenProject() { }
	void SaveSceneAs() { }
	void OnScenePlay() { }
	void OnScenePause() { }
	void OnSceneStop() { }
};

}