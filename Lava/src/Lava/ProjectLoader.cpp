#include "ProjectLoader.h"

#include <VolcaniCore/Core/Log.h>
#include <VolcaniCore/Core/Algo.h>
#include <VolcaniCore/Core/FileUtils.h>

#include <Magma/Core/YAMLSerializer.h>

#include "UIBrowser.h"
#include "UILoader.h"
#include "SceneLoader.h"

namespace fs = std::filesystem;

using namespace Magma;

namespace Lava {

static void LoadSource(YAML::Node& node, const std::string& srcPath);
static void LoadAssets(YAML::Node& node, const std::string& assetPath);

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

	proj.Name = projNode["Name"].as<std::string>();
	proj.App = projNode["App"].as<std::string>();
}

void ProjectLoader::Save(const Project& proj, const std::string& volcPath) {
	YAMLSerializer serializer;
	serializer.BeginMapping(); // File

	serializer.WriteKey("Project").BeginMapping();
		serializer
		.WriteKey("Name").Write(proj.Name)
		.WriteKey("App").Write(proj.App);

	serializer.EndMapping(); // Project
	serializer.EndMapping(); // File

	serializer.Finalize(volcPath);
}

void ProjectLoader::Compile(const std::string& volcPath) {
	YAML::Node file;
	try {
		file = YAML::LoadFile(volcPath);
	}
	catch(YAML::ParserException e) {
		return;
	}

	auto projNode = file["Project"];
	VOLCANICORE_ASSERT(projNode);

	auto fullPath = fs::canonical(volcPath);
	auto parentPath = fullPath.parent_path();
	auto name = parentPath.stem().string();
	auto rootPath = parentPath.string();
	auto srcPath   = (parentPath / "src").string();
	auto assetPath = (parentPath / "asset").string();

	UIBrowser::Load((fs::path(srcPath) / "UI").string());
	LoadSource(projNode, srcPath);
	// UIBrowser::Reload();

	// LoadAssets(projNode, assetPath);
}

void LoadSource(YAML::Node& node, const std::string& srcPath) {
	auto genPath = fs::path("Lava") / "projects" / "Project" / "gen";
	fs::remove_all(genPath);
	fs::create_directory(genPath);

	auto namespaceName = node["Name"].as<std::string>();
	auto appName = node["App"].as<std::string>();

	auto cppFile = File((genPath / "AppLoader").string() + ".cpp");

	std::string templateStr =
		"#include <VolcaniCore/Core/Defines.h>\n\n"
		"#include <App/{appName}.h>\n\n"
		"namespace Project {\n\n"
		"static Lava::App* s_App;\n\n"
		"extern \"C\" EXPORT Lava::App* GetApp() {\n"
		"\treturn s_App;\n"
		"}\n\n"
		"extern \"C\" EXPORT void LoadApp() {\n"
		"\ts_App = new " + namespaceName + "::{appName};\n"
		"}\n\n"
		"}";

	Replace(templateStr, "{appName}", appName);
	cppFile.Write(templateStr);

	std::string command;
#ifdef VOLCANICENGINE_LINUX
	command += "vendor/premake/bin/Linux/premake5.exe";

#elif VOLCANICENGINE_WINDOWS
	command += "vendor\\premake\\bin\\Windows\\premake5.exe";
	command += " gmake2 --file=Lava\\projects\\premake5.lua";
	command += " --src=\"" + srcPath + "\"";
	system(command.c_str());

	command = "powershell Start-Process mingw32-make.exe -NoNewWindow -Wait";
	command += " -WorkingDir Lava\\projects\\build";
	command += " -ArgumentList '-f Makefile';";
	system(command.c_str());
#endif

}

void LoadAssets(YAML::Node& node, const std::string& assetPath) {
	auto modelPath = (fs::path(assetPath) / "model").string();

	// for(auto path : FileUtils::GetFiles(modelPath)) {
	// 	AssetManager::GetOrCreate<Mesh>(path);
	// }
}

}