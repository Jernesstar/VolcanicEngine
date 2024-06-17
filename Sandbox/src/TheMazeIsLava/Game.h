#pragma once

#include <Core/Application.h>
#include <Renderer/Camera.h>

#include "Level.h"
#include "UI.h"

using namespace VolcaniCore;

namespace TheMazeIsLava {


class Game : public Application {
public:
	Game();
	~Game();

	void OnUpdate(TimeStep ts) override;

private:
	void Reset();

	void StartScreen();
	void LevelScreen();
	void PlayScreen();
	void PauseScreen();
	void OverScreen();

	void Save();
	void Load();

	void LoadUI();
	void LoadLevels();

private:
	Ref<UIElement> m_CurrentUI;
	Ref<Camera> m_Camera;
	TimeStep m_TimeStep;

	std::vector<Level> m_Levels;

	bool m_ReturnPressed = false;
	bool m_GameOver = false;
	uint32_t m_CurrentLevel = 0;
	std::function<void(void)> m_CurrentScreen;

	inline static Ref<UIElement> m_NoUI;
	inline static Ref<UIElement> m_HomeScreenUI;
	inline static Ref<UIElement> m_LevelSelectUI;
	inline static Ref<UIElement> m_PauseUI;
	inline static Ref<UIElement> m_OverUI;
};


}