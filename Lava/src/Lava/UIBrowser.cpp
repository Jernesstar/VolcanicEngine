#include "UIBrowser.h"

#include <VolcaniCore/Core/Log.h>
#include <VolcaniCore/Core/FileUtils.h>

#include <Magma/Core/DLL.h>

#include "UILoader.h"
#include "UIObject.h"

namespace fs = std::filesystem;

using namespace Magma;
using namespace Magma::UI;

namespace Lava {

static List<UIPage> s_Pages;
static UIPage* s_CurrentPage = nullptr;
static Ref<DLL> s_CurrentDLL = nullptr;

static Theme s_Theme;

void UIBrowser::Load(const std::string& folderPath) {
	auto genPath = fs::path("Lava") / "projects" / "UI" / "gen";
	fs::remove_all(genPath);
	fs::create_directory(genPath);

	auto contextIncludes = File("Lava/projects/UI/gen/Context.h");
	contextIncludes
	.Write("#pragma once\n")
	.Write("#include <Magma/UI/UI.h>")
	.Write("#include <Lava/UIObject.h>")
	.Write("#include <Lava/UIBrowser.h>")
	.Write("#include <VolcaniCore/Core/Log.h>")
	.Write("#include <VolcaniCore/Core/Input.h>")
	.Write("")
	.Write("using namespace Lava;")
	.Write("");

	auto srcPath = fs::path(folderPath).parent_path();

	auto app = srcPath / "App";
	auto _class = srcPath / "Class";
	auto common = srcPath / "Common";
	auto script = srcPath / "Script";

	for(auto dir : { app, _class, common, script })
		for(auto path : FileUtils::GetFiles(dir.string(), { ".h" }))
			contextIncludes.Write("#include \"" + path + "\"");

	auto projName = srcPath.parent_path().stem().string();
	contextIncludes
	.Write("")
	.Write("using namespace " + projName + ";")
	.Write("");

	auto premakeFile = std::ofstream("Lava/projects/UI/pages.lua"); // Deletes file contents

	auto themePath = (fs::path(folderPath) / "theme.magma.ui.json").string();
	if(FileUtils::FileExists(themePath))
		s_Theme = UILoader::LoadTheme(themePath);

	for(auto filePath : FileUtils::GetFiles(folderPath, { ".json" })) {
		fs::path p(filePath);
		auto name = p.stem().stem().stem().string();
		auto filePathName = (fs::path(folderPath) / name).string();

		if(name != "theme") {
			auto& page = s_Pages.emplace_back(name);
			page.SetTheme(s_Theme);
			UILoader::Load(page, filePathName);
			UILoader::Compile(filePathName);
		}
	}
}

void UIBrowser::Reload() {

}

void UIBrowser::OnUpdate(TimeStep ts) {
	if(!s_CurrentPage) {
		VOLCANICORE_LOG_INFO("Null");
		return;
	}

	auto get = s_CurrentDLL->GetFunction<UIObject*, std::string>("GetObject");

	s_CurrentPage->Traverse(
		[&](UIElement* element)
		{
			UIObject* object = get(element->GetID());
			if(!object)
				return;

			object->OnUpdate(ts);

			UIState state = element->GetState();
			if(state.Clicked)
				object->OnClick();
			if(state.Hovered)
				object->OnHover();
			if(state.MouseUp)
				object->OnMouseUp();
			if(state.MouseDown)
				object->OnMouseDown();
		});

	// TODO(Fix): Crash, the UIElements are somehow null
	s_CurrentPage->Render();
}

void UIBrowser::SetPage(const std::string& name) {
	s_CurrentPage = GetPage(name);
	s_CurrentDLL = UILoader::GetDLL(name);

	auto load = s_CurrentDLL->GetFunction<void, UIPage*>("LoadObjects");
	load(s_CurrentPage);
}

UIPage* UIBrowser::GetPage(const std::string& name) {
	if(name != "")
		for(auto& page : s_Pages)
			if(page.Name == name)
				return &page;

	return nullptr;
}

}