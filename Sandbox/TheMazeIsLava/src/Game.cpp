#include "Game.h"

#include <VolcaniCore/Core/Input.h>
#include <VolcaniCore/Core/Assert.h>
#include <VolcaniCore/Event/Events.h>

#include <VolcaniCore/Renderer/Renderer.h>
#include <VolcaniCore/Renderer/Renderer3D.h>
#include <VolcaniCore/Renderer/StereographicCamera.h>
#include <VolcaniCore/Renderer/ShaderLibrary.h>

#include <Magma/ECS/Systems.h>

#include "Asset.h"
#include "GameState.h"
#include "Player.h"
#include "IsometricCamera.h"
#include "Light.h"

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
	Asset::Init();
	GameState::Reset();

	// auto shader = ShaderPipeline::Create({
	// 	{ "Sandbox/assets/shaders/Lighting.glsl.vert", ShaderType::Vertex },
	// 	{ "Sandbox/assets/shaders/Lighting.glsl.frag", ShaderType::Fragment }
	// });
	auto shader = ShaderLibrary::Get("Mesh");
	m_LightingPass = RenderPass::Create("Lighting", shader);

	PointLight light =
		PointLight{
			.Constant  = 0.3f,
			.Linear    = 0.0f,
			.Quadratic = 0.032f,
		};
	light.Position = { 0.0f, 2.0f, 0.0f },
	light.Ambient  = { 0.2f, 0.2f, 0.2f },
	light.Diffuse  = { 0.5f, 0.5f, 0.5f },
	light.Specular = { 1.0f, 1.0f, 1.0f },

	// shader->Bind();

	// shader->SetInt("u_PointLightCount", 1);
	// shader->SetVec3("u_PointLights[0].Position", light.Position);
	// shader->SetVec3("u_PointLights[0].Ambient",  light.Ambient);
	// shader->SetVec3("u_PointLights[0].Diffuse",  light.Diffuse);
	// shader->SetVec3("u_PointLights[0].Specular", light.Specular);

	// shader->SetFloat("u_PointLights[0].Constant",  light.Constant);
	// shader->SetFloat("u_PointLights[0].Linear",    light.Linear);
	// shader->SetFloat("u_PointLights[0].Quadratic", light.Quadratic);

	// shader->SetTexture("u_Material.Diffuse", cube->GetMaterial().Diffuse, 0);
	// shader->SetTexture("u_Material.Specular", cube->GetMaterial().Specular, 1);
	// shader->SetFloat("u_Material.Shininess", 32.0f);

	m_CurrentUI = GameState::HomeUI;
	m_CurrentScreen = std::bind(&Game::HomeScreen, this);
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

void Game::HomeScreen() {
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

	if(GameState::SelectedLevel == 0)
		return;

	auto camera = CreateRef<IsometricCamera>();
	m_Controller = CreateRef<CameraController>(camera,
		MovementControls(
		ControlMap{
			{ Control::Forward,  Key::Invalid },
			{ Control::Backward, Key::Invalid },
		})
	);

	auto& currLevel = GameState::GetLevel();
	m_Scene = currLevel.Load();
	m_Scene->SetCamera(camera);

	Coordinate start = currLevel.GetPlayerStart();
	Player player(m_Scene->GetEntityWorld());
	player
	.Get<TransformComponent>().Translation = { start.x, 0.0f, start.y };

	// TODO(Implement): Collision with group
	// PhysicsSystem::RegisterForCollisionDetection(player, m_LavaGroup);

	m_CurrentUI = GameState::EmptyUI;
	m_CurrentScreen = std::bind(&Game::PlayScreen, this);
}

void Game::PlayScreen() {
	// Gameplay
	GameState::GetLevel().OnUpdate(m_TimeStep);
	m_Controller->OnUpdate(m_TimeStep);
	m_Scene->OnUpdate(m_TimeStep);

	Renderer::StartPass(m_LightingPass);
	{
		Renderer::Clear();

		m_Scene->OnRender();
	}
	Renderer::EndPass();

	if(m_GameOver) {
		m_CurrentUI = GameState::GameOverUI;
		m_CurrentScreen = std::bind(&Game::OverScreen, this);
		return;
	}
	if(m_LevelPassed) {
		m_LevelPassed = false;
		if(GameState::SelectedLevel == GameState::MaxLevel) {
			GameState::MaxLevel++;
			GameState::ResetUI();
		}
	}
	if(m_ReturnPressed) {
		m_ReturnPressed = false;
		m_Paused = !m_Paused;
		m_CurrentUI = m_Paused ? GameState::PauseUI : GameState::EmptyUI;
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