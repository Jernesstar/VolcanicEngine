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

	static void Init();

private:
	void Reset();

	void StartScreen();
	void LevelScreen();
	void PlayScreen();
	void PauseScreen();
	void OverScreen();

	void Save();
	void Load();

private:
	Ref<UIElement> m_CurrentUI;
	Ref<Camera> m_Camera;
	TimeStep m_TimeStep;

	std::vector<Level> m_Levels;

	bool m_ReturnPressed = false;
	bool m_GameOver = false;
	uint32_t m_CurrentLevel = 0;
	std::function<void(void)> m_CurrentScreen;

	inline static Ref<UIElement> s_NoUI;
	inline static Ref<UIElement> s_HomeScreenUI;
	inline static Ref<UIElement> s_LevelSelectUI;
	inline static Ref<UIElement> s_PauseUI;
	inline static Ref<UIElement> s_OverUI;
};


}