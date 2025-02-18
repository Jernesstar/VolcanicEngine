#include "ProjectLoader.h"

#include <VolcaniCore/Core/Log.h>
#include <VolcaniCore/Core/Algo.h>
#include <VolcaniCore/Core/FileUtils.h>

#include <Magma/Core/YAMLSerializer.h>

namespace fs = std::filesystem;

using namespace Magma;

namespace Lava {

static void LoadSource(YAML::Node& node, const std::string& srcPath);
static void LoadScenes(YAML::Node& node, const std::string& sceneFolder);

void ProjectLoader::Load(Project& proj, const std::string& volcPath) {
	YAML::Node file;
	try {
		file = YAML::LoadFile(volcPath);
	}
	catch(YAML::ParserException e) {
		return;
	}

	auto projNode = file["Project"];
	VOLCANICORE_ASSERT(projNode);

	proj.Path = fs::canonical(volcPath).parent_path().string();
	proj.ExportPath = projNode["ExportPath"].as<std::string>();
	proj.Name = projNode["Name"].as<std::string>();
	proj.App = projNode["App"].as<std::string>();
	proj.StartScreen = projNode["StartScreen"].as<std::string>();
	for(auto node : projNode["Screens"])
		proj.AddScreen(
			node["Screen"]["Name"].as<std::string>(),
			node["Screen"]["Scene"].as<std::string>(),
			node["Screen"]["UI"].as<std::string>());
}

void ProjectLoader::Save(const Project& proj, const std::string& volcPath) {
	YAMLSerializer serializer;
	serializer.BeginMapping(); // File

	serializer.WriteKey("Project")
	.BeginMapping()
		.WriteKey("ExportPath").Write(proj.ExportPath)
		.WriteKey("Name").Write(proj.Name)
		.WriteKey("App").Write(proj.App)
		.WriteKey("StartScreen").Write(proj.StartScreen);

	serializer.WriteKey("Screens").BeginSequence();
	for(const auto& screen : proj.Screens)
		serializer.BeginMapping()
		.WriteKey("Screen")
		.BeginMapping()
			.WriteKey("Name").Write(screen.Name)
			.WriteKey("Scene").Write(screen.Scene)
			.WriteKey("UI").Write(screen.Page)
		.EndMapping()
		.EndMapping();

	serializer.EndSequence();

	serializer
	.EndMapping(); // Project

	serializer.EndMapping(); // File

	serializer.Finalize(volcPath);
}

}