#pragma once

#include <Renderer/Model.h>

#include <Magma/UI/UI.h>

#include "Level.h"

using namespace VolcaniCore;
using namespace Magma;

namespace TheMazeIsLava {

// Global state
class GameState {
public:
	static void Load();
	static void Save();
	static void Reset();

	// static void SetUI();
	// static void Update(TimeStep ts);

	static Level& GetCurrentLevel() { return Levels[CurrentLevel - 1]; }

public:
	inline static const uint32_t LevelCount = 3;

	inline static uint32_t Coins;
	inline static uint32_t CurrentLevel;
	inline static std::vector<Level> Levels;

	inline static Ref<UI::UIElement> EmptyUI;
	inline static Ref<UI::UIElement> HomeUI;
	inline static Ref<UI::UIElement> LevelSelectUI;
	inline static Ref<UI::UIElement> PauseUI;
	inline static Ref<UI::UIElement> GameOverUI;

	inline static Ref<Texture> Stone;
	inline static Ref<Texture> Lava;
	// inline static Ref<Texture> Door;

	// One can unlock different playable characters
	inline static Ref<Model> PlayerModel1;

private:
	static void LoadState(bool newState = false);
	static void SaveState();
	static void InitUI();
	static void InitAssets();
};

}