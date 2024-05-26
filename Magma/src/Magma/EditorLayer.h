#pragma once

#include <filesystem>

#include <VolcaniCore/Core/Time.h>
#include <VolcaniCore/Renderer/Texture.h>

#include "Scene.h"

namespace Magma {

class EditorLayer {
public:
	EditorLayer();
	~EditorLayer();

	void Update(VolcaniCore::TimeStep ts);
	void Render();

private:
	Ref<Scene> m_CurrentScene;
	Ref<Texture> m_IconPlay, m_IconPause, m_IconStop;

	enum class SceneState { Edit, Play };
	SceneState m_SceneState = SceneState::Edit;

	bool m_ViewportFocused;
	bool m_ViewportHovered;
	glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
	glm::vec2 m_ViewportBounds[2];

	// void UI_Toolbar();

	void NewScene();
	void OpenScene();
	void OpenScene(const std::filesystem::path& path);
	void SaveScene();
	void SaveSceneAs() { }
	void OpenProject() { }

	void OnSceneSimulate() { }

};

}