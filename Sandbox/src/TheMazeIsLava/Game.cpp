#include "Game.h"

#include <fstream>

#include <yaml-cpp/yaml.h>

#include <Core/Input.h>
#include <Core/Assert.h>
#include <Renderer/Renderer.h>
#include <Renderer/StereographicCamera.h>
#include <Events/EventSystem.h>

#include <OpenGL/Renderer.h>

using namespace VolcaniCore;

namespace TheMazeIsLava {

void Game::Init() {
	s_NoUI			= CreateRef<UIEmpty>();
	s_LevelSelectUI = CreateRef<UIWindow>(600, 400,
		glm::vec4{ 0.859375f, 0.76171875f, 0.5859375f, 1.0f },
		glm::vec4{ 0.3125f, 0.234375f, 0.078125f, 1.0f }, 10, 20
	);
	s_PauseUI = CreateRef<UIWindow>(600, 400,
		glm::vec4{ 0.859375f, 0.76171875f, 0.5859375f, 1.0f },
		glm::vec4{ 0.3125f, 0.234375f, 0.078125f, 1.0f }, 10, 20
	);
	s_OverUI = CreateRef<UIWindow>(600, 400,
		glm::vec4{ 0.859375f, 0.76171875f, 0.5859375f, 1.0f },
		glm::vec4{ 0.3125f, 0.234375f, 0.078125f, 1.0f }, 10, 20
	);

	Level::Init();
}

Game::Game() {
	EventSystem::RegisterListener<KeyPressedEvent>(
	[&](const KeyPressedEvent& event) {
		if(event.Key == Key::Escape)
			Application::Close();
		if(event.Key == Key::Return && event.IsRepeat == false)
			m_ReturnPressed = true;
	});

	Reset();
	Load();

	m_Camera = CreateRef<StereographicCamera>(75.0f, 0.01f, 100.0f, 800, 600);
	m_Camera->SetPosition({ 0.0f, 0.0f, 3.0f });

	VOLCANICORE_LOG_INFO("Going to Start Screen");
	m_CurrentUI = s_NoUI;
	m_CurrentScreen = std::bind(&Game::StartScreen, this);
}

Game::~Game() {
	Save();
}

void Game::OnUpdate(TimeStep ts) {
	m_TimeStep = ts;

	Renderer::Clear();
	Application::GetRenderer()->As<OpenGL::Renderer>()->Begin(m_Camera);

	m_CurrentScreen();
	m_CurrentUI->Render();

	Application::GetRenderer()->As<OpenGL::Renderer>()->End();
}

void Game::StartScreen() {
	// Front picture, level load, settings

	if(m_ReturnPressed) {
		m_ReturnPressed = false;
		VOLCANICORE_LOG_INFO("Going to Level Screen");
		m_CurrentUI = s_LevelSelectUI;
		m_CurrentScreen = std::bind(&Game::LevelScreen, this);
		return;
	}
}

void Game::LevelScreen() {
	// Staircase like level selection

	if(m_ReturnPressed) {
		VOLCANICORE_LOG_INFO("Going to Play Screen");
		m_ReturnPressed = false;
		m_CurrentLevel = 1;
		m_CurrentUI = s_NoUI;
		m_CurrentScreen = std::bind(&Game::PlayScreen, this);
		return;
	}
}

void Game::PlayScreen() {
	// Gameplay

	m_Levels[m_CurrentLevel - 1].Render(m_TimeStep);

	if(m_GameOver) {
		VOLCANICORE_LOG_INFO("Going to Over Screen");
		m_CurrentUI = s_OverUI;
		m_CurrentScreen = std::bind(&Game::OverScreen, this);
		return;
	}
	if(m_ReturnPressed) {
		VOLCANICORE_LOG_INFO("Going to Pause Screen");
		m_ReturnPressed = false;
		m_CurrentUI = s_PauseUI;
		m_CurrentScreen = std::bind(&Game::PauseScreen, this);
		return;
	}
}

void Game::PauseScreen() {
	if(m_ReturnPressed) {
		VOLCANICORE_LOG_INFO("Exiting Pause Screen; Going to Play Screen");
		m_ReturnPressed = false;
		m_CurrentUI = s_NoUI;
		m_CurrentScreen = std::bind(&Game::PlayScreen, this);
		return;
	}
}

void Game::OverScreen() {
	if(m_ReturnPressed) {
		VOLCANICORE_LOG_INFO("Exiting Over Screen; Going to Play Screen");
		m_ReturnPressed = false;
		m_CurrentUI = s_NoUI;
		m_CurrentScreen = std::bind(&Game::PlayScreen, this);
		return;
	}
}

void Game::Save() {
	YAML::Emitter out;
	out << YAML::BeginMap; // File

	out << YAML::Key << "Current Level" << YAML::Value << m_CurrentLevel;

	out << YAML::EndMap; // File
	std::ofstream fout("Sandbox/assets/saves/progress.save");
	fout << out.c_str();
}

void Game::Load() {
	YAML::Node file;
	try {
		file = YAML::LoadFile("Sandbox/assets/saves/progress.save");
	}
	catch (YAML::ParserException e) {
		return;
	}

	m_CurrentLevel = file["Current Level"].as<uint32_t>();
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

}