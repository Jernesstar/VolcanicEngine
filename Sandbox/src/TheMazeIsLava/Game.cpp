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


Game::Game()
{
	EventSystem::RegisterListener<KeyPressedEvent>(
	[&](const KeyPressedEvent& event) {
		if(event.Key == Key::Escape)
			Application::Close();
		if(event.Key == Key::Return && event.IsRepeat == false)
			m_ReturnPressed = true;
	});

	m_Camera->SetPositionDirection({ 5.0f, 2.0f, 5.0f }, { -0.5f, -0.5f, -0.5f });

	Level::Init();

	LoadLevels();
	LoadUI();
	Load();

	m_CurrentUI = m_HomeScreenUI;
	m_CurrentScreen = std::bind(&Game::StartScreen, this);
	VOLCANICORE_LOG_INFO("Going to Start Screen");
}

Game::~Game() {
	Save();
}

void Game::OnUpdate(TimeStep ts) {
	m_TimeStep = ts;
	m_CameraController.OnUpdate(ts);

	Renderer::Clear();
	Application::GetRenderer()->As<OpenGL::Renderer>()->Begin(m_Camera);

	m_CurrentScreen();
	m_CurrentUI->Render();
}

void Game::StartScreen() {
	// Front picture, level load, settings

	if(m_ReturnPressed) {
		m_ReturnPressed = false;
		m_CurrentUI = m_LevelSelectUI;
		m_CurrentScreen = std::bind(&Game::LevelScreen, this);
		VOLCANICORE_LOG_INFO("Going to Level Screen");
		return;
	}
}

void Game::LevelScreen() {
	// Staircase like level selection

	if(m_ReturnPressed) {
		m_ReturnPressed = false;
		m_CurrentLevel = 1;
		m_CurrentUI = m_NoUI;
		m_CurrentScreen = std::bind(&Game::PlayScreen, this);
		VOLCANICORE_LOG_INFO("Going to Play Screen");
		return;
	}
}

void Game::PlayScreen() {
	// Gameplay

	m_Levels[m_CurrentLevel - 1].Render(m_TimeStep);

	if(m_GameOver) {
		m_CurrentUI = m_OverUI;
		m_CurrentScreen = std::bind(&Game::OverScreen, this);
		VOLCANICORE_LOG_INFO("Going to Over Screen");
		return;
	}
	if(m_ReturnPressed) {
		m_ReturnPressed = false;
		m_CurrentUI = m_PauseUI;
		m_CurrentScreen = std::bind(&Game::PauseScreen, this);
		VOLCANICORE_LOG_INFO("Going to Pause Screen");
		return;
	}
}

void Game::PauseScreen() {
	if(m_ReturnPressed) {
		m_ReturnPressed = false;
		m_CurrentUI = m_NoUI;
		m_CurrentScreen = std::bind(&Game::PlayScreen, this);
		VOLCANICORE_LOG_INFO("Exiting Pause Screen; Going to Play Screen");
		return;
	}
}

void Game::OverScreen() {
	if(m_ReturnPressed) {
		m_ReturnPressed = false;
		m_CurrentUI = m_NoUI;
		m_CurrentScreen = std::bind(&Game::PlayScreen, this);
		VOLCANICORE_LOG_INFO("Exiting Over Screen; Going to Play Screen");
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

void Game::LoadUI() {
	m_NoUI = CreateRef<UIEmpty>();
	m_HomeScreenUI = CreateRef<UIWindow>(600, 400,
		glm::vec4{ 0.859375f, 0.76171875f, 0.5859375f, 1.0f },
		glm::vec4{ 0.3125f, 0.234375f, 0.078125f, 1.0f }, 10, 20
	);
	m_LevelSelectUI = CreateRef<UIWindow>(600, 200,
		glm::vec4{ 0.859375f, 0.76171875f, 0.5859375f, 1.0f },
		glm::vec4{ 0.3125f, 0.234375f, 0.078125f, 1.0f }, 10, 20
	);
	m_PauseUI = CreateRef<UIWindow>(600, 400,
		glm::vec4{ 0.859375f, 0.76171875f, 0.5859375f, 1.0f },
		glm::vec4{ 0.3125f, 0.234375f, 0.078125f, 1.0f }, 10, 20
	);
	m_OverUI = CreateRef<UIWindow>(600, 400,
		glm::vec4{ 0.859375f, 0.76171875f, 0.5859375f, 1.0f },
		glm::vec4{ 0.3125f, 0.234375f, 0.078125f, 1.0f }, 10, 20
	);

	m_HomeScreenUI->Add(CreateRef<UIButton>(70, 50, glm::vec4{ 0.3125f, 0.234375f, 0.078125f, 0.01f }));
}

void Game::LoadLevels() {
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