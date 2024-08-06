#include "Game.h"

#include <Core/Input.h>
#include <Core/Assert.h>
#include <Events/EventSystem.h>

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
	EventSystem::RegisterListener<KeyPressedEvent>(
	[&](const KeyPressedEvent& event) {
		if(event.Key == Key::Escape)
			Application::Close();
		if(event.Key == Key::Return && event.IsRepeat == false)
			m_ReturnPressed = true;
	});

	UI::Init();
	GameState::Reset();

	m_Scene = GameState::LoadLevel()

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

	// TODO:
	// DrawStairs();

	if(GameState::GetSelectedLevel() == 0)
		return;

	m_Scene = GameState::GetSelectedLevel().Load();
	m_CurrentUI = GameState::EmptyUI;
	m_CurrentScreen = std::bind(&Game::PlayScreen, this);
}

void Game::PlayScreen() {
	// Gameplay

	if(m_GameOver) {
		m_CurrentUI = GameState::GameOverUI;
		m_CurrentScreen = std::bind(&Game::OverScreen, this);
		return;
	}
	if(m_ReturnPressed) {
		m_ReturnPressed = false;
		m_Paused = true;
		m_CurrentUI = GameState::PauseUI;
		return;
	}
	if(m_ReturnPressed) {
		m_ReturnPressed = false;
		m_CurrentUI = GameState::EmptyUI;
		m_CurrentScreen = std::bind(&Game::PlayScreen, this);
		return;
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