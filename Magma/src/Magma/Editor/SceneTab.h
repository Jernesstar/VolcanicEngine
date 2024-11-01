#pragma once

#include "Tab.h"

#include "Scene/Scene.h"

#include "UI/Button.h"

using namespace VolcaniCore;

namespace Magma {

class SceneTab : public Tab {
public:
	SceneTab(Ref<Scene> scene);
	~SceneTab();

	void Update(VolcaniCore::TimeStep ts) override;
	void Render() override;

private:
	Ref<Scene> m_Scene;
	Ref<UI::Button> m_PlayButton, m_PauseButton, m_StopButton;
	
	enum class SceneState { Edit, Play, Pause };
	SceneState m_SceneState = SceneState::Edit;

	void NewScene();
	void OpenScene();
	void SaveScene();

	void AddEntity();

	void ToolbarUI();
	void OnScenePlay();
	void OnScenePause();
	void OnSceneStop();
};

}