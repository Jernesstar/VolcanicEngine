#pragma once

#include <Magma/Scene/Scene.h>

#include "Editor/Tab.h"

#include "UI/Button.h"

using namespace VolcaniCore;

namespace Magma {

enum class ScreenState { Edit, Play, Pause };

struct RunConfiguration {
	struct SceneConfiguration {
		std::string Name;
		std::string Screen;
		std::string UI;
	};

	struct UIConfiguration {
		std::string Name;
		std::string Screen;
		std::string Scene;
	};

	List<SceneConfiguration> SceneConfigs;
	List<UIConfiguration> UIConfigs;
};

class ProjectTab : public Tab {
public:
	ProjectTab(const std::string& path);
	~ProjectTab();

	void Update(TimeStep ts) override;
	void Render() override;
	void RenderButtons();
	void RenderEssentialPanels();

	void OnPlay();
	void OnPause();
	void OnResume();
	void OnStop();

	ScreenState GetState() const { return m_ScreenState; }

private:
	UI::Button m_PlayButton, m_PauseButton, m_ResumeButton, m_StopButton;
	ScreenState m_ScreenState = ScreenState::Edit;
	RunConfiguration m_Configs;

private:
	void Setup();
};

}