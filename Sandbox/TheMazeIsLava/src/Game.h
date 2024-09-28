#pragma once

#include <Core/Application.h>

#include <Renderer/CameraController.h>

#include "Screen.h"

using namespace VolcaniCore;
using namespace Magma;

namespace TheMazeIsLava {

class Game : public Application {
public:
	Game();
	~Game();

	void OnUpdate(TimeStep ts) override;

private:
	void Play();
	void LoadScreens();

private:
	Screen* m_CurrentScreen;

	Screen EmptyScreen;
	Screen HomeScreen;
	Screen LevelScreen;
	Screen PlayScreen;
	Screen PauseScreen;
	Screen OverScreen;
};

}