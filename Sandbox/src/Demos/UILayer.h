#pragma once

namespace Demo {

class UILayer : public Application {
public:
	UILayer();
	~UILayer();

	void OnUpdate(TimeStep ts);

	void ToolbarUI();

private:
	Ref<UI::UIElement> Root;
	Ref<UI::Button> m_PlayButton, m_PauseButton, m_StopButton;

	enum class SceneState { Edit, Play, Pause };
	SceneState m_SceneState = SceneState::Edit;
};

void UILayer::ToolbarUI() {
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));

	auto flags = ImGuiWindowFlags_NoDecoration
			   | ImGuiWindowFlags_NoScrollbar
			   | ImGuiWindowFlags_NoScrollWithMouse;
	ImGui::Begin("##toolbar", nullptr, flags);
	{
		float size = ImGui::GetWindowHeight() - 2.0f;
		auto x = 0.5f*ImGui::GetWindowContentRegionMax().x - 0.5f*size;
		auto button = m_PlayButton;
		if(m_SceneState == SceneState::Play)
			button = m_PauseButton;

		button->x = x;
		button->y = ImGui::GetCursorPosY();
		button->SetSize(size, size);
		button->Render();

		ImGui::SameLine();
		m_StopButton->x = ImGui::GetCursorPosX() + 5.0f;
		m_StopButton->y = ImGui::GetCursorPosY();
		m_StopButton->SetSize(size, size);
		m_StopButton->Render();
	}
	ImGui::End();

	ImGui::PopStyleColor();
	ImGui::PopStyleVar(2);
}

UILayer::UILayer()
	: Application(1920, 1080)
{
	UI::Init();

	Events::RegisterListener<KeyPressedEvent>(
		[](const KeyPressedEvent& event)
		{
			if(event.Key == Key::Escape)
				Application::Close();
		});

	m_PlayButton = UI::Button::Create(
		{
			.Image = "Magma/assets/icons/PlayButton.png",
			.x = 100, .y = 100,
			.Width = 20, .Height = 20,
			// .OnPressed = [this]() { OnScenePlay(); }
		});
	m_PauseButton = UI::Button::Create(
		{
			.Image = "Magma/assets/icons/PauseButton.png",
			.x = 100, .y = 100,
			.Width = 20, .Height = 20,
			// .OnPressed = [this]() { OnScenePause(); }
		});
	m_StopButton = UI::Button::Create(
		{
			.Image = "Magma/assets/icons/StopButton.png",
			.x = 100, .y = 100,
			.Width = 20, .Height = 20,
			// .OnPressed = [this]() { OnSceneStop(); }
		});

	// Root = UI::UIBuilder<UI::Window>(
	// 	UI::Window::Specification
	// 	{
	// 		.Width = 600,
	// 		.Height = 600,
	// 		.x = 100, .y = 100,
	// 		.Color = { 0.4f, 0.9f, 0.1f, 1.0f },
	// 	})
	// .Add<UI::Button>(
	// 	{
	// 		// .Color = { 0.0f, 1.0f, 1.0f, 1.0f },
	// 		// .Text = "Magma/assets/icons/PlayButton.png",
	// 		.Image = "Magma/assets/icons/PlayButton.png",
	// 		.x = 100, .y = 100,
	// 		.Width = 100, .Height = 100
	// 	})
	// .Add<UI::Text>(
	// 	{
	// 		.Text = "Root UI",
	// 		.Color = { 0.0f, 1.0f, 0.0f, 1.0f },
	// 		.x = 20, .y = 200
	// 	})
	// .Add<UI::Image>(
	// 	{
	// 		.Path = "Sandbox/assets/images/stone.png",
	// 		.x = 20, .y = 400,
	// 		.Width = 100, .Height = 100
	// 	})
	// .Finalize();

	VOLCANICORE_LOG_INFO("UI project is now running");
}

UILayer::~UILayer() {
	UI::Close();
}

void UILayer::OnUpdate(TimeStep ts) {
	UI::Begin();

	ToolbarUI();
	// Root->Render();

	UI::End();
}

}