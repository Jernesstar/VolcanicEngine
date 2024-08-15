#include "GameState.h"

#include <fstream>

#include <yaml-cpp/yaml.h>

#include <Core/Log.h>

#include <Magma/UI/UIElements.h>

namespace TheMazeIsLava {

void GameState::Load() {
	LoadState();
	InitUI();

	ResetUI();
}

void GameState::Reset() {
	LoadState(true);
	InitUI();

	ResetUI();
}

void GameState::Save() {
	SaveState();
}

Level& GameState::GetLevel(uint32_t level) {
	if(level == 0)
		return Levels[SelectedLevel - 1];
	return Levels[level - 1];
}

void GameState::InitUI() {
	EmptyUI = UI::Empty::Create();
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
	HomeUI		  = UI::Window::Create(specs);
	LevelSelectUI = UI::Window::Create(specs);
	PauseUI		  = UI::Window::Create(specs);
	GameOverUI	  = UI::Window::Create(specs);

	HomeUI
	->Add<UI::Text>("The Maze is Lava", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f))
	->SetPosition(160, 100);

	HomeUI
	->Add<UI::Text>("Press start to play", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f))
	->SetPosition(120, 150);

	// HomeUI
	// ->Add<UI::Image>("Sandbox/assets/images/background.png")
	// ->SetPosition(100, 100);
}

void GameState::ResetUI() {
	LevelSelectUI->Clear();

	float offset = 0.0f;
	for(uint32_t i = 1; i <= Levels.size(); i++) {
		glm::vec4 color = { 0.3125f, 0.234375f, 0.078125f, 1.0f };
		if(i > MaxLevel)
			color.a = 0.7f; // Buttons for locked levels are darker

		LevelSelectUI
		->Add<UI::Button>(color, std::to_string(i))
		->SetOnPressed(
		[i]() {
			if(i <= MaxLevel)
				SelectedLevel = i;
		})
		->SetSize(70, 50)
		->SetPosition(i * 70 + (offset += 40.0f), 100.0f);
	}
}

Level LoadLevel(YAML::Node levelNode) {
	std::vector<std::vector<uint32_t>> tilemap;
	for(auto rowNode : levelNode["Tilemap"])
		tilemap.push_back(rowNode.as<std::vector<uint32_t>>());

	return Level{ levelNode["Name"].as<std::string>(), tilemap };
}

void GameState::LoadState(bool newState) {
	std::string path = newState ? "Sandbox/TheMazeIsLava/assets/saves/new.save"
								: "Sandbox/TheMazeIsLava/assets/saves/game.save";
	YAML::Node file;
	try {
		file = YAML::LoadFile(path);
	}
	catch(YAML::ParserException e) {
		return;
	}
	auto save = file["Save"];

	MaxLevel = save["Current Level"].as<uint32_t>();

	for(auto node : save["Levels"])
		Levels.push_back(LoadLevel(node["Level"]));
}

void SaveLevel(YAML::Emitter& out, Level& level) {
	out << YAML::Key << "Level" << YAML::Value << YAML::BeginMap; // Level
	out << YAML::Key << "Name" << YAML::Value << level.Name;

	out << YAML::Key << "Tilemap" << YAML::Value << YAML::BeginSeq; // Tilemap
	for(auto row : level.Tilemap)
		out << row;
	out << YAML::EndSeq; // Tilemap

	out << YAML::EndMap; // Level
}

void GameState::SaveState() {
	YAML::Emitter out;
	out << YAML::BeginMap; // File
	out << YAML::Key << "Save" << YAML::Value << YAML::BeginMap; // Save

	out << YAML::Key << "Current Level" << YAML::Value << MaxLevel;

	out << YAML::Key << "Levels" << YAML::Value << YAML::BeginSeq; // Levels
	for(auto level : Levels) {
		out << YAML::BeginMap; // Level
		SaveLevel(out, level);
		out << YAML::EndMap; // Level
	}
	out << YAML::EndSeq; // Levels

	out << YAML::EndMap; // Save
	out << YAML::EndMap; // File

	std::ofstream fout("Sandbox/TheMazeIsLava/assets/saves/game.save");
	fout << out.c_str();
}

}