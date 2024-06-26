#include "GameState.h"

#include <fstream>

#include <yaml-cpp/yaml.h>

namespace TheMazeIsLava {

void GameState::Load() {
	LoadState();
	InitUI();

	PlayerModel1 = CreateRef<Model>("TheMazeIsLave/assets/models/player.fbx");
}

void GameState::Save() {
	SaveState();
}

void GameState::Reset() {
	LoadState(true);
	InitUI();
}

void GameState::InitUI() {
	EmptyUI = CreateRef<UI::Empty>();
	HomeUI = CreateRef<UI::Window>(600, 400,
		glm::vec4{ 0.859375f, 0.76171875f, 0.5859375f, 1.0f },
		glm::vec4{ 0.3125f, 0.234375f, 0.078125f, 1.0f }, 10, 20
	);
	LevelSelectUI = CreateRef<UI::Window>(600, 200,
		glm::vec4{ 0.859375f, 0.76171875f, 0.5859375f, 1.0f },
		glm::vec4{ 0.3125f, 0.234375f, 0.078125f, 1.0f }, 10, 20
	);
	PauseUI = CreateRef<UI::Window>(600, 400,
		glm::vec4{ 0.859375f, 0.76171875f, 0.5859375f, 1.0f },
		glm::vec4{ 0.3125f, 0.234375f, 0.078125f, 1.0f }, 10, 20
	);
	GameOverUI = CreateRef<UI::Window>(600, 400,
		glm::vec4{ 0.859375f, 0.76171875f, 0.5859375f, 1.0f },
		glm::vec4{ 0.3125f, 0.234375f, 0.078125f, 1.0f }, 10, 20
	);

	HomeScreenUI
	->Add<UI::Button>({ 0.3125f, 0.234375f, 0.078125f, 1.0f })
	->SetSize(70, 50)
	->SetBorder(CreateRef<Texture>("Sandbox/TheMazeIsLava/assets/border.png"));

	for(uint32_t i = 0; i < LevelCount; i++) {
		glm::vec4 color = { 0.3125f, 0.234375f, 0.078125f, 1.0f };
		if(i > CurrentLevel)
			color.a = 0.2f; // Buttons for locked levels are darker
		LevelSelectUI
		->Add<UI::Button>(color, std::to_string(i), glm::vec4(1.0f))
		->SetPosition(i, 0.0f)
		->SetOnPressed(
		[&i]() {
			VOLCANICORE_LOG_INFO("Yet another button pressed");
		});
	}
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
	std::string path = newState ? "Sandbox/assets/saves/new.save"
								: "Sandbox/assets/saves/game.save";
	YAML::Node file;
	try {
		file = YAML::LoadFile(path);
	}
	catch(YAML::ParserException e) {
		return;
	}
	auto save = file["Save"];

	m_CurrentLevel = save["Current Level"].as<uint32_t>();
	m_Coins = save["Coins"].as<uint32_t>();

	for(auto node : save["Levels"]) {
		m_Levels.push_back(LoadLevel(node["Level"]));
	}
}

void SaveLevel(YAML::Emitter& out, Level& level) {
	out << YAML::Key << "Level" << YAML::Value << YAML::BeginMap; // Level
	out << YAML::Key << "Name" << YAML::Value << level.Name;

	out << YAML::Key << "Tilemap" << YAML::Value << YAML::BeginSeq; // Tilemap
	for(auto row : m_Levels)
		out << row;
	out << YAML::EndSeq; // Tilemap

	out << YAML::EndMap; // Level
}

void GameState::SaveState() {
	YAML::Emitter out;
	out << YAML::BeginMap; // File
	out << YAML::Key << "Save" << YAML::Value << YAML::BeginMap; // Save

	out << YAML::Key << "Current Level" << YAML::Value << m_CurrentLevel;
	out << YAML::Key << "Coins" << YAML::Value << m_Coins;

	out << YAML::Key << "Levels" << YAML::Value << YAML::BeginSeq; // Levels
	for(auto level : m_Levels) {
		out << YAML::BeginMap; // Level
		SaveLevel(level, out);
		out << YAML::EndMap; // Level
	}
	out << YAML::EndSeq; // Levels

	out << YAML::EndMap; // Save
	out << YAML::EndMap; // File

	std::ofstream fout("Sandbox/assets/saves/game.save");
	fout << out.c_str();
}


}