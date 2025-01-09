#pragma once

#include "Tab.h"

#include "Scene/Scene.h"

#include "UI/Button.h"

using namespace VolcaniCore;

namespace Magma {

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

private:
	Scene m_Scene;
	std::string m_ScenePath;
	Ref<UI::Button> m_PlayButton, m_PauseButton, m_StopButton;

	enum class SceneState { Edit, Play, Pause };
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