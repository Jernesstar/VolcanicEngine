#include "GameState.h"

#include <fstream>

#define YAML_CPP_STATIC_DEFINE
#include <yaml-cpp/yaml.h>

#include <Core/Log.h>

#include <Magma/UI/UI.h>

namespace TheMazeIsLava {

void GameState::Load() {
	LoadState();
}

void GameState::Reset() {
	LoadState(true);
}

void GameState::Save() {
	SaveState();
}

Level& GameState::GetLevel(uint32_t level) {
	if(level == 0)
		return Levels[SelectedLevel - 1];
	return Levels[level - 1];
}

Level LoadLevel(YAML::Node levelNode) {
	std::string name = levelNode["Name"].as<std::string>();

	List<List<uint32_t>> tilemap;
	for(auto rowNode : levelNode["Tilemap"])
		tilemap.push_back(rowNode.as<List<uint32_t>>());

	return Level{ name, tilemap };
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

	out << YAML::Key << "Tilemap" << YAML::BeginSeq; // Tilemap
	for(auto row : level.Map) {
		out << YAML::Flow;
		out << row;
	}
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