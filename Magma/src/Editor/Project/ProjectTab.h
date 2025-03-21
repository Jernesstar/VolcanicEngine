#pragma once

#include <Magma/Scene/Scene.h>

#include "Editor/Tab.h"

#include "UI/Button.h"

using namespace VolcaniCore;

namespace Magma {

enum class ScreenState { Edit, Play, Pause };

class ProjectTab : public Tab {
public:
	ProjectTab();
	ProjectTab(const std::string& path);
	~ProjectTab();

	void Update(TimeStep ts) override;
	void Render() override;
	void RenderButtons();
	void RenderEssentialPanels();

	ScreenState GetState() const { return m_ScreenState; }

private:
	UI::Button m_PlayButton, m_PauseButton, m_StopButton;
	ScreenState m_ScreenState = ScreenState::Edit;

private:
	void Setup();

	void OnPlay();
	void OnPause();
	void OnStop();
};

}