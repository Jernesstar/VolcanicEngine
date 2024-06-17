#pragma once

#include <Core/Application.h>
#include <Renderer/Camera.h>
#include <Renderer/StereographicCamera.h>
#include <Renderer/CameraController.h>

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
	Ref<Camera> m_Camera = CreateRef<StereographicCamera>(75.0f, 0.01f, 100.0f, 800, 600);
	CameraController m_CameraController{ m_Camera };

	std::vector<Level> m_Levels;

	TimeStep m_TimeStep;
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