#include "Game.h"

#include <VolcaniCore/Core/Input.h>
#include <VolcaniCore/Core/Assert.h>
#include <VolcaniCore/Event/Events.h>
#include <VolcaniCore/Renderer/RendererAPI.h>

#include <Magma/ECS/EntityBuilder.h>

#include "Asset.h"
#include "GameState.h"
#include "Player.h"
#include "IsometricCamera.h"
#include "Light.h"

using namespace Magma::ECS;

namespace TheMazeIsLava {

Game::Game()
	: Application(800, 600, "The Maze is Lava")
{
	Events::RegisterListener<KeyPressedEvent>(
		[&](const KeyPressedEvent& event)
		{
			if(event.Key == Key::Escape)
				Application::Close();
		});

	UI::Init();
	GameState::Reset();

	Asset::Init();

	LoadScreens();

	m_CurrentScreen = &HomeScreen;
	m_CurrentScreen->OnLoad();
}

Game::~Game() {
	GameState::Save();
	UI::Close();
}

void Game::OnUpdate(TimeStep ts) {
	RendererAPI::Get()->Clear();
	m_CurrentScreen->OnUpdate(ts);
	m_CurrentScreen->OnRender();
}

void Game::LoadScreens() {
	Ref<UI::UIElement> home, level, pause, over; 

	UI::Window::Specification specs{
		.Width = 600,
		.Height = 400,
		.x = 100.0f,
		.y = 100.0f,
		.Color = glm::vec4{ 0.859375f, 0.76171875f, 0.5859375f, 1.0f },
		.BorderWidth = 10,
		.BorderHeight = 20,
		.BorderColor = glm::vec4{ 0.3125f, 0.234375f, 0.078125f, 1.0f }
	};
	home  = UI::Window::Create(specs);
	level = UI::Window::Create(specs);
	pause = UI::Window::Create(specs);
	over  = UI::Window::Create(specs);

	home
	->Add<UI::Text>("The Maze is Lava", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f))
	->SetPosition(160, 100);
	home
	->Add<UI::Text>("Press start to play", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f))
	->SetPosition(120, 150);
	// home
	// ->Add<UI::Image>("Sandbox/assets/images/background.png")
	// ->SetPosition(100, 100);

	pause
	->Add<UI::Text>("Paused", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f))
	->SetPosition(160, 100);
	pause
	->Add<UI::Text>("Press enter to resume", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f))
	->SetPosition(120, 150);

	EmptyScreen = Screen{ };
	HomeScreen	= Screen{ home };
	LevelScreen = Screen{ level };
	PlayScreen  = Screen{ };
	PauseScreen = Screen{ pause };
	OverScreen	= Screen{ over };

	HomeScreen.OnUpdate =
		[&](TimeStep ts)
		{
 			auto& state = HomeScreen.GetState();

			if(state.ReturnPressed) {
				state.ReturnPressed = false;
				m_CurrentScreen = &LevelScreen;
				m_CurrentScreen->OnLoad();
			}
		};

	LevelScreen.OnLoad =
		[&]()
		{
			auto ui = LevelScreen.GetUI();
			ui->Clear();

			float offset = 0.0f;
			for(uint32_t i = 1; i <= GameState::Levels.size(); i++) {
				glm::vec4 color = { 0.3125f, 0.234375f, 0.078125f, 1.0f };
				if(i > GameState::MaxLevel)
					color.a = 0.7f;

				ui
				->Add<UI::Button>(color, std::to_string(i))
				->SetOnPressed(
				[i]() {
					if(i <= GameState::MaxLevel)
						GameState::SelectedLevel = i;
				})
				->SetSize(70, 50)
				->SetPosition(i * 70 + (offset += 40.0f), 100.0f);
			}
		};

	LevelScreen.OnUpdate =
		[&](TimeStep _)
		{
			// TODO(Implement): Staircase like level selection
			// DrawLevelVisualizer();

			if(GameState::SelectedLevel == 0)
				return;

			m_CurrentScreen = &PlayScreen;
			m_CurrentScreen->OnLoad();
		};

	PlayScreen.OnLoad =
		[&]()
		{
			auto& currLevel = GameState::GetLevel();
			currLevel.Load();
			auto scene = currLevel.GetScene();

			auto camera = CreateRef<IsometricCamera>();
			auto& controller = scene->GetRenderer().GetCameraController();
			controller.SetControls(
				MovementControls(
					ControlMap{
						{ Control::Up,   Key::W },
						{ Control::Down, Key::S },
						{ Control::Forward,  Key::Invalid },
						{ Control::Backward, Key::Invalid },
					})
				);
			controller.TranslationSpeed = 5.0f;
			controller.RotationSpeed = 0.0f;
			controller.SetCamera(camera);

			EntityBuilder(scene->EntityWorld, "MainCamera")
			.Add<CameraComponent>()
			.Finalize();

			auto [x, y] = currLevel.PlayerStart;
			Player player(scene->EntityWorld);
			player.Get<TransformComponent>().Translation = { x, 0.0f, y };
			player.Get<TransformComponent>().Scale = glm::vec3(1.0f);

			// TODO(Implement): Collision with group
			// PhysicsSystem::RegisterForCollisionDetection(player, m_LavaGroup);
		};

	PlayScreen.OnUpdate =
		[&](TimeStep ts)
		{
			auto& state = HomeScreen.GetState();
			auto& level = GameState::GetLevel();

			level.OnUpdate(ts);
			level.OnRender();

			if(level.GameOver) {
				m_CurrentScreen = &OverScreen;
			}
			else if(level.Complete) {
				m_CurrentScreen = &LevelScreen;

				if(GameState::SelectedLevel == GameState::MaxLevel) {
					GameState::MaxLevel++;

					m_CurrentScreen->OnLoad();
				}
			}
			else if(state.ReturnPressed) {
				state.ReturnPressed = false;
				level.Paused = true;
				m_CurrentScreen = &PauseScreen;
			}
		};

	// TODO(Fix): Have Pause UI be part of the PlayScreen
	PauseScreen.OnUpdate =
		[&](TimeStep _)
		{
			auto& state = HomeScreen.GetState();
			auto& level = GameState::GetLevel();

			if(state.ReturnPressed) {
				state.ReturnPressed = false;
				level.Paused = false;
				m_CurrentScreen = &PlayScreen;
				m_CurrentScreen->OnLoad();
			}
		};

	OverScreen.OnUpdate =
		[&](TimeStep _)
		{
			auto& state = HomeScreen.GetState();

			if(state.ReturnPressed) {
				state.ReturnPressed = false;
				m_CurrentScreen = &PlayScreen;
				m_CurrentScreen->OnLoad();
			}
		};
}

}