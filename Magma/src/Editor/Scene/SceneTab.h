#pragma once

#include <Magma/Scene/Scene.h>

#include "Editor/Tab.h"

#include "UI/Button.h"

using namespace VolcaniCore;

namespace Magma {

enum class SceneState { Edit, Play, Pause };

class SceneTab : public Tab {
public:
	SceneTab();
	SceneTab(const Scene& scene);
	SceneTab(const std::string& path);
	~SceneTab();

	void Update(TimeStep ts) override;
	void Render() override;

	void SetScene(const Scene& scene);
	void SetScene(const std::string& path);

	SceneState GetState() const { return m_SceneState; }

private:
	Scene m_Scene;
	std::string m_ScenePath;
	Ref<UI::Button> m_PlayButton, m_PauseButton, m_StopButton;

	SceneState m_SceneState = SceneState::Edit;

private:
	void Setup();

	void ToolbarUI();

	void NewScene();
	void OpenScene();
	void SaveScene();

	void AddEntity();

	void OnScenePlay();
	void OnScenePause();
	void OnSceneStop();
};

}