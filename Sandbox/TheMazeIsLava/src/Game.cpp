#include "Game.h"

#include <Core/Input.h>
#include <Core/Assert.h>
#include <Event/Events.h>

#include <Renderer/Renderer.h>
#include <Renderer/Renderer3D.h>
#include <Renderer/StereographicCamera.h>

#include "GameState.h"

using namespace VolcaniCore;
using namespace Magma;

namespace TheMazeIsLava {

Game::Game()
	: Application(800, 600, "The Maze is Lava")
{
	Events::RegisterListener<KeyPressedEvent>(
	[&](const KeyPressedEvent& event) {
		if(event.Key == Key::Escape)
			Application::Close();
		if(event.Key == Key::Return && event.IsRepeat == false)
			m_ReturnPressed = true;
	});

	UI::Init();
	GameState::Reset();

	m_CurrentUI = GameState::HomeUI;
	m_CurrentScreen = std::bind(&Game::StartScreen, this);
}

Game::~Game() {
	GameState::Save();
	UI::Close();
}

void Game::OnUpdate(TimeStep ts) {
	m_TimeStep = ts;

	Renderer::Clear();

	m_CurrentScreen();

	UI::Begin();
	m_CurrentUI->Render();
	UI::End();
}

void Game::StartScreen() {
	// // Front picture, level load, settings

	if(m_ReturnPressed) {
		m_ReturnPressed = false;
		m_CurrentUI = GameState::LevelSelectUI;
		m_CurrentScreen = std::bind(&Game::LevelScreen, this);
		return;
	}
}

void Game::LevelScreen() {
	// Staircase like level selection

	// TODO(Implement):
	// DrawLevelVisualizer();

	uint32_t level;
	if((level = GameState::GetSelectedLevel()) == 0)
		return;

	m_Scene = GameState::GetLevel(level).Load();
	m_CurrentUI = GameState::EmptyUI;
	m_CurrentScreen = std::bind(&Game::PlayScreen, this);
}

void Game::PlayScreen() {
	// Gameplay

	m_Scene->OnUpdate(m_TimeStep);
	m_Scene->OnRender();

	if(m_GameOver) {
		m_CurrentUI = GameState::GameOverUI;
		m_CurrentScreen = std::bind(&Game::OverScreen, this);
		return;
	}
	if(m_ReturnPressed) {
		m_ReturnPressed = false;
		m_Paused = !m_Paused;
		m_CurrentUI = (m_Paused ? GameState::PauseUI : GameState::EmptyUI);
	}
}

void Game::OverScreen() {
	if(m_ReturnPressed) {
		m_ReturnPressed = false;
		m_CurrentUI = GameState::EmptyUI;
		m_CurrentScreen = std::bind(&Game::PlayScreen, this);
		return;
	}
}

}