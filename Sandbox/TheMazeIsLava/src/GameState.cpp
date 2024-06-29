#include "GameState.h"

#include <fstream>

#include <yaml-cpp/yaml.h>

#include <Core/Log.h>

#include <Magma/UI/UIElements.h>

namespace TheMazeIsLava {


void GameState::Load() {
	LoadState();
	InitAssets();
	InitUI();
}

void GameState::Save() {
	SaveState();
}

void GameState::Reset() {
	LoadState(true);
	InitAssets();
	InitUI();
}

void GameState::InitUI() {
	EmptyUI = CreateRef<UI::Empty>();
	HomeUI = CreateRef<UI::Window>(
		600, 400, glm::vec4{ 0.859375f, 0.76171875f, 0.5859375f, 1.0f },
		10, 20, glm::vec4{ 0.3125f, 0.234375f, 0.078125f, 1.0f }
	);
	LevelSelectUI = CreateRef<UI::Window>(
		600, 200, glm::vec4{ 0.859375f, 0.76171875f, 0.5859375f, 1.0f },
		10, 20, glm::vec4{ 0.3125f, 0.234375f, 0.078125f, 1.0f }
	);
	PauseUI = CreateRef<UI::Window>(
		600, 400, glm::vec4{ 0.859375f, 0.76171875f, 0.5859375f, 1.0f },
		10, 20, glm::vec4{ 0.3125f, 0.234375f, 0.078125f, 1.0f }
	);
	GameOverUI = CreateRef<UI::Window>(
		600, 400, glm::vec4{ 0.859375f, 0.76171875f, 0.5859375f, 1.0f },
		10, 20, glm::vec4{ 0.3125f, 0.234375f, 0.078125f, 1.0f }
	);

	// HomeUI
	// ->Add<UI::Button>(glm::vec4{ 0.3125f, 0.234375f, 0.078125f, 1.0f })
	// ->SetSize(70, 50)
	// ->SetBorder(
	// 		Texture::Create("Sandbox/TheMazeIsLava/assets/images/border.png"));

	// for(uint32_t i = 0; i < LevelCount; i++) {
	// 	glm::vec4 color = { 0.3125f, 0.234375f, 0.078125f, 1.0f };
	// 	if(i > CurrentLevel)
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

void GameState::InitAssets() {
	Stone = Texture::Create("Sandbox/TheMazeIsLava/assets/images/stone.png");
	// Lava  = Texture::Create("Sandbox/TheMazeIsLava/assets/images/lava.png");
	// Door  = Texture::Create("Sandbox/TheMazeIsLava/assets/images/door.png");
	// PlayerModel1 = Model::Create(
	// 					"Sandbox/TheMazeIsLava/assets/models/player.fbx");
}

YAML::Emitter& operator <<(YAML::Emitter& out, std::vector<uint32_t>& row) {
	out << YAML::Flow;
	out << YAML::BeginSeq;
	for(auto tile : row)
		out << tile;
	out << YAML::EndSeq;
	return out;
}

Level LoadLevel(YAML::Node levelNode) {
	std::vector<std::vector<uint32_t>> tilemap;
	for(auto rowNode : levelNode["Tilemap"])
		tilemap.push_back(rowNode.as<std::vector<uint32_t>>());

	return Level{ levelNode["Name"].as<std::string>(), tilemap };
}

void GameState::LoadState(bool newState) {
	std::string path = newState ? "Sandbox/TheMazeIsLava/assets/saves/new.save"
								: "Sandbox/TheMazeIsLava/assets/saves/curr.save";
	YAML::Node file;
	try {
		file = YAML::LoadFile(path);
	}
	catch(YAML::ParserException e) {
		return;
	}
	auto save = file["Save"];

	CurrentLevel = save["Current Level"].as<uint32_t>();
	Coins = save["Coins"].as<uint32_t>();

	for(auto node : save["Levels"])
		Levels.push_back(LoadLevel(node["Level"]));
}

void SaveLevel(YAML::Emitter& out, Level& level) {
	out << YAML::Key << "Level" << YAML::Value << YAML::BeginMap; // Level
	out << YAML::Key << "Name" << YAML::Value << level.Name;

	out << YAML::Key << "Tilemap" << YAML::Value << YAML::BeginSeq; // Tilemap
	for(auto row : level.GetTilemap())
		out << row;
	out << YAML::EndSeq; // Tilemap

	out << YAML::EndMap; // Level
}

void GameState::SaveState() {
	YAML::Emitter out;
	out << YAML::BeginMap; // File
	out << YAML::Key << "Save" << YAML::Value << YAML::BeginMap; // Save

	out << YAML::Key << "Current Level" << YAML::Value << CurrentLevel;
	out << YAML::Key << "Coins" << YAML::Value << Coins;

	out << YAML::Key << "Levels" << YAML::Value << YAML::BeginSeq; // Levels
	for(auto level : Levels) {
		out << YAML::BeginMap; // Level
		SaveLevel(out, level);
		out << YAML::EndMap; // Level
	}
	out << YAML::EndSeq; // Levels

	out << YAML::EndMap; // Save
	out << YAML::EndMap; // File

	std::ofstream fout("Sandbox/assets/saves/game.save");
	fout << out.c_str();
}


}