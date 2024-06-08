#include "Game.h"

#include <fstream>

#include <yaml-cpp/yaml.h>

#include <Core/Input.h>
#include <Renderer/Renderer.h>

namespace TheMazeIsLava {

Game::Game() {
	Reset();
	Load();
	m_CurrentScreen = StartScreen;
}

Game::~Game() {
	Save();
}

void Game::OnUpdate(TimeStep ts) {
	m_TimeStep = ts;
	m_CurrentScreen();
}

void Game::Reset() {
	m_Levels.push_back(Level{
	{
		{ 0, 0, 0, 0, 0, 0, 0, 0, 3, 0 },
		{ 0, 0, 2, 0, 1, 1, 1, 0, 1, 0 },
		{ 0, 1, 1, 0, 1, 0, 1, 1, 1, 0 },
		{ 0, 1, 0, 0, 1, 0, 0, 0, 0, 0 },
		{ 0, 1, 1, 1, 1, 1, 1, 1, 1, 0 },
		{ 0, 0, 0, 0, 1, 1, 0, 0, 2, 0 },
		{ 0, 0, 0, 0, 0, 1, 0, 1, 1, 0 },
		{ 0, 1, 1, 1, 0, 1, 0, 1, 0, 0 },
		{ 0, 2, 0, 1, 1, 1, 0, 1, 2, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	}, [](float t) { return 8.0f; } });
	m_Levels.push_back(Level{
	{
		{ 0, 0, 0, 0, 0, 0, 0, 0, 3, 0 },
		{ 0, 0, 1, 1, 1, 1, 1, 0, 1, 0 },
		{ 0, 0, 1, 0, 0, 0, 1, 0, 1, 0 },
		{ 0, 0, 1, 0, 2, 0, 1, 0, 1, 0 },
		{ 0, 0, 1, 0, 1, 0, 1, 0, 1, 0 },
		{ 0, 0, 1, 0, 1, 0, 1, 0, 1, 0 },
		{ 0, 0, 1, 0, 1, 1, 1, 0, 1, 0 },
		{ 0, 0, 1, 0, 0, 0, 0, 0, 1, 0 },
		{ 0, 0, 1, 1, 1, 1, 1, 1, 1, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	}, [](float t) { return 5.99f; } });
	m_Levels.push_back(Level{
	{
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 3 },
		{ 0, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 0, 1, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 1, 1, 1, 1, 1, 1, 1, 1, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 1, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 1, 0 },
		{ 0, 1, 1, 1, 1, 1, 1, 1, 1, 0 },
		{ 0, 1, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 1, 1, 1, 1, 1, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 2, 0, 0, 0, 0 },
	}, [](float t) { return 5.85f; } });
}

void Game::StartScreen() {
	// Front picture, level load, settings
	// m_StartUI->Render();

	m_CurrentLevel = 1;
	m_CurrentScreen = PlayScreen;
}

void Game::LevelScreen() {
	// Staircase like level selection

	// m_LevelSelectUI->Render();
}

void Game::PlayScreen() {
	if(Input::KeyPressed(Key::Enter))
		m_CurrentScreen = PauseScreen;

	// Gameplay
	Renderer::Clear();

	Renderer::Begin(m_Camera)
	m_Levels[m_CurrentLevel - 1].Render(ts);
	Renderer::End();
}

void Game::PauseScreen() {
	// m_PauseUI->Render();

	if(Input::KeyPressed(Key::Enter))
		m_CurrentScreen = PlayScreen;
}

void Game::OverScreen() {
	// m_OverUI->Render();
}

void Game::Save() {
	// YAML::Emitter out;
	// out << YAML::BeginMap; // File

	// out << YAML::Key << "Current Level" << YAML::Value << m_CurrentLevel;

	// out << YAML::EndMap; // File
	// std::ofstream fout("Sandbox/assets/saves/progress.save");
	// fout << out.c_str();
}

void Game::Load() {
	YAML::Node file;
	try {
		file = YAML::LoadFile("Sandbox/assets/saves/progress.save");
	}
	catch (YAML::ParserException e) {
		VOLCANICORE_ASSERT_ARGS(false, "Could not load save file {0}: {1}", filepath, e.what());
	}
}

}