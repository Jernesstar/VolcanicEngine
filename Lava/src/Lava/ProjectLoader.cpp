#include "ProjectLoader.h"

#include <VolcaniCore/Core/Log.h>
#include <VolcaniCore/Core/Algo.h>
#include <VolcaniCore/Core/FileUtils.h>

#include <Magma/Core/YAMLSerializer.h>

#include "UILoader.h"
#include "SceneLoader.h"

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
	proj.Name = projNode["Name"].as<std::string>();
	proj.App = projNode["App"].as<std::string>();
}

void ProjectLoader::Save(const Project& proj, const std::string& volcPath) {
	YAMLSerializer serializer;
	serializer.BeginMapping(); // File

	serializer.WriteKey("Project")
	.BeginMapping()
		.WriteKey("Name").Write(proj.Name)
		.WriteKey("App").Write(proj.App)
	.EndMapping(); // Project

	serializer.EndMapping(); // File

	serializer.Finalize(volcPath);
}

void ProjectLoader::Compile(const Project& project) {
	auto parentPath = fs::path(project.Path);
	auto volcPath = (parentPath / ".volc.proj").string();

	YAML::Node file;
	try {
		file = YAML::LoadFile(volcPath);
	}
	catch(YAML::ParserException e) {
		return;
	}

	auto projNode = file["Project"];
	VOLCANICORE_ASSERT(projNode);

	auto name = parentPath.stem().string();
	auto projectPath = (parentPath / "Project").string();
	auto visualPath = (parentPath / "Visual");

	UILoader::CompileFolder((visualPath / "UI").string());
	LoadSource(projNode, projectPath);

	// LoadScenes(projNode, (visualPath / "Scene").string());
}

void LoadSource(YAML::Node& node, const std::string& projectPath) {
	auto genPath = fs::path("Lava") / "projects" / "Project" / "gen";

	if(!fs::is_directory(genPath) || !fs::exists(genPath))
		fs::create_directory(genPath);
	else
		for(auto path : FileUtils::GetFiles(genPath.string()))
			std::remove(path.c_str());

	auto namespaceName = node["Name"].as<std::string>();
	auto appName = node["App"].as<std::string>();

	FileUtils::CreateFile((genPath / "AppLoader.cpp").string());
	auto cppFile = File((genPath / "AppLoader.cpp").string());

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
	cppFile.Close();

	std::string command;
#ifdef VOLCANICENGINE_WINDOWS
	command = "vendor\\premake\\bin\\Windows\\premake5.exe gmake2";
	command += " --file=Lava\\projects\\premake5.lua";
	command += " --path=\"" + projectPath + "\"";
	system(command.c_str());

	command = "powershell Start-Process mingw32-make.exe -NoNewWindow -Wait";
	command += " -WorkingDir Lava\\projects\\build";
	command += " -ArgumentList '-f Makefile';";
	system(command.c_str());
#elif VOLCANICENGINE_LINUX
	command = "vendor/premake/bin/Linux/premake5.exe gmake2";
	command += " --file=Lava/projects/premake5.lua";
	command += " --path=\"" + projectPath + "\"";
	system(command.c_str());

	command = "cd Lava/projects/build; make -f Makefile;";
	system(command.c_str());
#endif
}

void LoadScenes(YAML::Node& node, const std::string& sceneFolder) {

}

Ref<DLL> ProjectLoader::GetDLL() {
	std::string path = "Lava/projects/Project/build/lib/";
#ifdef VOLCANICENGINE_WINDOWS
	path += "Loader.dll";
#elif VOLCANICENGINE_LINUX
	path = "./" + path + "libLoader.so";
#endif
	return CreateRef<DLL>(path);
}

}