#pragma once

#include <Magma/UI/UI.h>
#include <Magma/Scene/Scene.h>

#include "Level.h"

using namespace VolcaniCore;
using namespace Magma;

namespace TheMazeIsLava {

class GameState {
public:
	static void Load();
	static void Reset();
	static void Save();

	static Level& GetLevel(uint32_t level = 0);

public:
	inline static uint32_t MaxLevel; // The highest level reached so far
	inline static uint32_t SelectedLevel = 0; // The one being played right now
	inline static std::vector<Level> Levels;

private:
	static void LoadState(bool newState = false);
	static void SaveState();
};

}