#include "Project.h"

#include <VolcaniCore/Core/Log.h>
#include <VolcaniCore/Core/Algo.h>
#include <VolcaniCore/Core/FileUtils.h>

#include "YAMLSerializer.h"

namespace Magma {

void Project::Load(const std::string& path) {
	namespace fs = std::filesystem;

	YAML::Node file;
	try {
		file = YAML::LoadFile(path);
	}
	catch(YAML::ParserException e) {
		VOLCANICORE_LOG_INFO("File '%s' is not well formatted", path.c_str());
		return;
	}
	catch(YAML::BadFile e) {
		VOLCANICORE_LOG_INFO("File '%s' is bad", path.c_str());
		return;
	}

	auto projNode = file["Project"];
	VOLCANICORE_ASSERT(projNode);

	Path = fs::canonical(path).parent_path().string();
	ExportPath = projNode["ExportPath"].as<std::string>();
	Name = projNode["Name"].as<std::string>();
	App = projNode["App"].as<std::string>();
	StartScreen = projNode["StartScreen"].as<std::string>();
	Screens.Clear();
	for(auto node : projNode["Screens"])
		AddScreen(
			node["Screen"]["Name"].as<std::string>(),
			node["Screen"]["Scene"].as<std::string>(),
			node["Screen"]["UI"].as<std::string>());
}

void Project::Save(const std::string& exportPath) {
	YAMLSerializer serializer;
	serializer.BeginMapping(); // File

	serializer.WriteKey("Project")
	.BeginMapping()
		.WriteKey("ExportPath").Write(ExportPath)
		.WriteKey("Name").Write(Name)
		.WriteKey("App").Write(App)
		.WriteKey("StartScreen").Write(StartScreen);

	serializer.WriteKey("Screens").BeginSequence();
	for(const auto& screen : Screens)
		serializer.BeginMapping()
		.WriteKey("Screen")
		.BeginMapping()
			.WriteKey("Name").Write(screen.Name)
			.WriteKey("Scene").Write(screen.Scene)
			.WriteKey("UI").Write(screen.UI)
		.EndMapping()
		.EndMapping();

	serializer.EndSequence();

	serializer
	.EndMapping(); // Project

	serializer.EndMapping(); // File

	serializer.Finalize(exportPath);
}

void Project::AddScreen(const std::string& name,
						const std::string& scene, const std::string& ui)
{
	Screens.Emplace(name, scene, ui);
}

}