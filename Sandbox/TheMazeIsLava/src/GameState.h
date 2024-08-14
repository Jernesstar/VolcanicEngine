#pragma once

#include <Magma/UI/UI.h>
#include <Magma/Editor/Scene.h>

#include "Level.h"

using namespace VolcaniCore;
using namespace Magma;

namespace TheMazeIsLava {

// Global state
class GameState {
public:
	static void Load();
	static void Reset();
	static void Save();

	static void ResetUI();

	static Level& GetLevel(uint32_t level = 0);

public:
	inline static uint32_t MaxLevel; // The highest level reached so far
	inline static uint32_t SelectedLevel; // The one being played right now
	inline static std::vector<Level> Levels;

	inline static Ref<UI::UIElement> EmptyUI;
	inline static Ref<UI::UIElement> HomeUI;
	inline static Ref<UI::UIElement> LevelSelectUI;
	inline static Ref<UI::UIElement> PauseUI;
	inline static Ref<UI::UIElement> GameOverUI;

private:
	static void LoadState(bool newState = false);
	static void SaveState();
	static void InitUI();
};

}