#pragma once

#include <Core/Application.h>
#include <Core/Log.h>
#include <Events/EventSystem.h>

#include <Magma/UI/UIElements.h>

using namespace VolcaniCore;
using namespace Magma;

namespace Demo {

class GameUI : public Application {
public:
	GameUI();
	~GameUI();

	void OnUpdate(TimeStep ts);

private:
	Ref<UI::Empty> EmptyUI = CreateRef<UI::Empty>();
	Ref<UI::Window> HomeUI = CreateRef<UI::Window>(
		600, 400, glm::vec4{ 0.859375f, 0.76171875f, 0.5859375f, 1.0f },
		10, 20, glm::vec4{ 0.3125f, 0.234375f, 0.078125f, 1.0f }
	);
	Ref<UI::Window> LevelSelectUI = CreateRef<UI::Window>(
		600, 400, glm::vec4{ 0.859375f, 0.76171875f, 0.5859375f, 1.0f },
		10, 20, glm::vec4{ 0.3125f, 0.234375f, 0.078125f, 1.0f }
	);
	Ref<UI::Window> PauseUI = CreateRef<UI::Window>(
		600, 400, glm::vec4{ 0.859375f, 0.76171875f, 0.5859375f, 1.0f },
		10, 20, glm::vec4{ 0.3125f, 0.234375f, 0.078125f, 1.0f }
	);
	Ref<UI::Window> GameOverUI = CreateRef<UI::Window>(
		600, 400, glm::vec4{ 0.859375f, 0.76171875f, 0.5859375f, 1.0f },
		10, 20, glm::vec4{ 0.3125f, 0.234375f, 0.078125f, 1.0f }
	);

	Ref<UI::Button> button;
	Ref<UI::Text> text;
	Ref<UI::Image> image;
};

GameUI::GameUI() {
	EventSystem::RegisterListener<KeyPressedEvent>(
	[](const KeyPressedEvent& event) {
		if(event.Key == Key::Escape)
			Application::Close();
	});

	UI::Init();

	// button = CreateRef<UI::Button>(
	// 	glm::vec4{ 0.3125f, 0.234375f, 0.078125f, 1.0f }
	// );
	// text = CreateRef<UI::Text>("Just some test text");
	image = CreateRef<UI::Image>("Sandbox/assets/images/start_bg.png");

	HomeUI
	->SetPosition(100, 100);
	LevelSelectUI
	->SetPosition(100, 100);
	PauseUI
	->SetPosition(100, 100)
	->Add(image)
	->SetSize(600, 400);
	GameOverUI
	->SetPosition(100, 100);

	// HomeUI
	// ->Add<UI::Button>(glm::vec4{ 0.3125f, 0.234375f, 0.078125f, 1.0f })
	// ->SetSize(70, 50)
	// ->SetBorder(
	// 	Texture::Create("Sandbox/TheMazeIsLava/assets/images/border.png"));

	// for(uint32_t i = 0; i < 4; i++) {
	// 	glm::vec4 color = { 0.3125f, 0.234375f, 0.078125f, 1.0f };
	// 	if(i > 1)
	// 		color.a = 0.2f; // Buttons for locked levels are darker
	// 	LevelSelectUI
	// 	->Add<UI::Button>(color)
	// 	// ->SetText(std::to_string(i), glm::vec4(1.0f))
	// 	->SetOnPressed(
	// 	[&i]() {
	// 		// Prepare to play the next level
	// 	})
	// 	->SetPosition(i, 0.0f);
	// }
}

GameUI::~GameUI() {
	UI::Close();
}

void GameUI::OnUpdate(TimeStep ts) {
	Renderer::Clear();
	UI::Begin();

	// HomeUI->Render();
	PauseUI->Render();
	
	UI::End();
}

}