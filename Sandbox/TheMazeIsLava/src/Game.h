#pragma once

#include <Core/Application.h>
#include <Renderer/Camera.h>
#include <Renderer/StereographicCamera.h>
#include <Renderer/CameraController.h>

#include <Magma/Scene/Scene.h>
#include <Magma/UI/UI.h>

#include "Level.h"

using namespace VolcaniCore;
using namespace Magma;

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

private:
	Ref<UI::UIElement> m_CurrentUI;
	Ref<Scene> m_Scene;
	Ref<StereographicCamera> m_Camera;

	TimeStep m_TimeStep;
	std::function<void(void)> m_CurrentScreen;
	uint32_t m_CurrentLevel = 0;
	bool m_ReturnPressed = false;
	bool m_GameOver = false;
};

}