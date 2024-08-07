#pragma once

#include <Object/Model.h>

#include <Object/Mesh.h>
#include <Object/Shader.h>

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
	static void Save();
	static void Reset();

	static Level& GetLevel(uint32_t level) { return Levels[level - 1]; }
	static uint32_t GetSelectedLevel() { return SelectedLevel; }

public:
	inline static const uint32_t LevelCount = 3;

	inline static uint32_t MaxLevel = 1; // The highest level reached so far
	inline static uint32_t SelectedLevel = 0; // The one being played right now
	inline static std::vector<Level> Levels;

	inline static Ref<UI::UIElement> EmptyUI;
	inline static Ref<UI::UIElement> HomeUI;
	inline static Ref<UI::UIElement> LevelSelectUI;
	inline static Ref<UI::UIElement> PauseUI;
	inline static Ref<UI::UIElement> GameOverUI;

	inline static Ref<Mesh> Wall;

	inline static Ref<ShaderPipeline> MeshShader;

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