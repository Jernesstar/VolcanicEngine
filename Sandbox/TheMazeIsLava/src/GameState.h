#pragma once

#include "UI.h"

using namespace VolcaniCore;
using namespace Magma;

namespace TheMazeIsLava {

// Global state
class GameState {
public:
	void Load();
	void Save();
	void Reset();

	void SetUI();
	void Update(TimeStep ts);

	uint32_t GetCurrentLevel();
	Ref<Scene> LoadLevel(Level level);

public:
	inline static const uint32_t LevelCount = 3;

	inline static Ref<UI::UIElement> EmptyUI;
	inline static Ref<UI::UIElement> HomeUI;
	inline static Ref<UI::UIElement> LevelSelectUI;
	inline static Ref<UI::UIElement> PauseUI;
	inline static Ref<UI::UIElement> GameOverUI;

	inline static Ref<Texture> Lava;
	inline static Ref<Texture> Stone;
	inline static Ref<Texture> Door;

	// One can unlock different playable characters
	inline static Ref<Model> PlayerModel1;

private:
	static void InitUI();
	static void LoadState(bool newState);
	static void SaveState();

	uint32_t m_CurrentLevel;
	std::vector<Level> m_Levels;
};

}