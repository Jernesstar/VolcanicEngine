#pragma once

#include <Core/Application.h>

#include <Renderer/RenderPass.h>
#include <Renderer/Camera.h>
#include <Renderer/CameraController.h>

#include <Magma/Editor/Scene.h>
#include <Magma/UI/UI.h>

using namespace VolcaniCore;
using namespace Magma;

namespace TheMazeIsLava {

class Game : public Application {
public:
	Game();
	~Game();

	void OnUpdate(TimeStep ts) override;

private:
	void HomeScreen();
	void LevelScreen();
	void PlayScreen();
	void OverScreen();

private:
	TimeStep m_TimeStep;
	Ref<UI::UIElement> m_CurrentUI;
	std::function<void(void)> m_CurrentScreen;

	Ref<Scene> m_Scene;
	Ref<CameraController> m_Controller;

	inline static Ref<RenderPass> m_LightingPass;
	inline static Ref<RenderPass> m_ShadowPass;

	bool m_Paused = false;
	bool m_GameOver = false;
	bool m_LevelPassed = false;
	bool m_ReturnPressed = false;
};

}