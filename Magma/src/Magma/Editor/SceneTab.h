#pragma once

#include "Tab.h"

#include "Scene/Scene.h"

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
	Ref<Texture> m_IconPlay, m_IconPause, m_IconStop;
	
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