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

static Map<std::string, Ref<DLL>> s_DLLs;

static List<UIPage> s_Pages;
static UIPage* s_CurrentPage = nullptr;

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

	auto filePaths = FileUtils::GetFiles(folderPath, { ".json" });
	s_Pages.reserve(filePaths.size());
	for(auto filePath : filePaths) {
		fs::path p(filePath);
		auto name = p.stem().stem().stem().string();
		auto filePathName = (fs::path(folderPath) / name).string();

		if(name != "theme") {
			auto& page = s_Pages.emplace_back(name);
			page.SetTheme(s_Theme);
			UILoader::Load(page, filePathName);
			UILoader::Compile(filePathName);
			break;
		}
	}

	s_CurrentPage = &s_Pages[0];
}

void UIBrowser::Reload() {
	// for(auto& page : s_Pages) {
	// 	s_DLLs[page.Name] = UILoader::GetDLL(page.Name);
	// 	auto load = s_DLLs[page.Name]->GetFunction<void, UIPage*>("LoadObjects");
	// 	load(&page);
	// }
}

void UIBrowser::OnUpdate(TimeStep ts) {
	if(!s_CurrentPage)
		return;

	// auto gen = s_DLLs[s_CurrentPage->Name];
	// auto get = gen->GetFunction<UIObject*, std::string>("GetObject");

	// s_CurrentPage->Traverse(
	// 	[&](UIElement* element)
	// 	{
	// 		UIObject* object = get(element->GetID());
	// 		if(!object)
	// 			return;

	// 		object->OnUpdate(ts);

	// 		UIState state = element->GetState();
	// 		if(state.Clicked)
	// 			object->OnClick();
	// 		if(state.Hovered)
	// 			object->OnHover();
	// 		if(state.MouseUp)
	// 			object->OnMouseUp();
	// 		if(state.MouseDown)
	// 			object->OnMouseDown();
	// 	});

	s_CurrentPage->Render();
}

void UIBrowser::SetPage(const std::string& name) {
	s_CurrentPage = GetPage(name);
}

UIPage* UIBrowser::GetPage(const std::string& name) {
	if(name != "")
		for(auto& page : s_Pages)
			if(page.Name == name)
				return &page;

	return nullptr;
}

}