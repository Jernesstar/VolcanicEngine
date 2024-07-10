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
	
	m_Camera = CreateRef<StereographicCamera>(75.0f, 0.01f, 100.0f, 800, 600);
	m_Camera->SetPositionDirection(
		{  5.0f,  2.0f,  5.0f },
		{ -0.5f, -0.5f, -0.5f }
	);

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

	UI::Begin();
	Renderer3D::Begin(m_Camera);
	GameState::MeshShader->SetMat4("u_ViewProj", m_Camera->GetViewProjection());

	m_CurrentScreen();
	m_CurrentUI->Render();

	Renderer3D::End();
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
	// // Staircase like level selection

	if(m_ReturnPressed) {
		m_ReturnPressed = false;
		m_CurrentUI = GameState::EmptyUI;
		m_CurrentScreen = std::bind(&Game::PlayScreen, this);
		return;
	}
}

void Game::PlayScreen() {
	// // Gameplay
	GameState::GetCurrentLevel().Render(m_TimeStep);

	if(m_GameOver) {
		m_CurrentUI = GameState::GameOverUI;
		m_CurrentScreen = std::bind(&Game::OverScreen, this);
		return;
	}
	if(m_ReturnPressed) {
		m_ReturnPressed = false;
		m_CurrentUI = GameState::PauseUI;
		m_CurrentScreen = std::bind(&Game::PauseScreen, this);
		return;
	}
}

void Game::PauseScreen() {
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