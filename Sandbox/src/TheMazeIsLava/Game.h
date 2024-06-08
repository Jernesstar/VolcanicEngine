#pragma once

#include <Core/Application.h>

#include "Level.h"

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

private:
	Ref<Camera> m_Camera;
	std::vector<Level> m_Levels;

	TimeStep m_TimeStep;
	uint32_t m_CurrentLevel = 0;
	void (*m_CurrentScreen)();
};

}