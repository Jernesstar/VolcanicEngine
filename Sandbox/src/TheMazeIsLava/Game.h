#pragma once

#include <Core/Application.h>

#include "Level.h"

using namespace VolcaniCore;

namespace TheMazeIsLava {

class Game : public Application {
public:
	Game();

	void OnUpdate(TimeStep ts) override;

	void Reset();
	void Play();
	void Pause();

private:
	std::vector<Level> m_Levels;
};

}