#include "UIBrowser.h"

#include <VolcaniCore/Core/FileUtils.h>

#include <Magma/Core/DLL.h>

#include "UILoader.h"
#include "UIObject.h"

namespace fs = std::filesystem;

using namespace Magma;
using namespace Magma::UI;

namespace Lava {

static std::string s_UIFolderPath;
static Map<std::string, Ref<DLL>> s_DLLs;

static List<UIPage> s_Pages;
static UIPage* s_CurrentPage;

static Map<UIElementType, ThemeElement> s_Theme;

void UIBrowser::Load(const std::string& folderPath) {
	s_UIFolderPath = folderPath;

	auto themePath = (fs::path(folderPath) / "theme.magma.ui.json").string();
	if(FileUtils::FileExists(themePath))
		s_Theme = UILoader::LoadTheme(themePath);

	for(auto filePath : FileUtils::GetFiles(folderPath, { ".json" })) {
		fs::path p(filePath);
		auto name = p.stem().stem().stem().string();
		auto filePathName = (fs::path(folderPath) / name).string();

		if(name == "theme")
			continue;
		else {
			UILoader::Load(s_Pages.emplace_back(name), filePathName);
			UILoader::Compile(filePathName);
		}
	}

	s_CurrentPage = &s_Pages[0];
}

void UIBrowser::Reload() {
	for(auto& page : s_Pages) {
		auto path = fs::path("Lava") / "projects" / "UI" / "build" / page.Name;
		s_DLLs[page.Name] = CreateRef<DLL>((path / "UI.dll").string());

		if(&page == s_CurrentPage) {
			auto load = s_DLLs[page.Name]->GetFunction<void>("LoadObjects");
			load();
		}
	}
}

void UIBrowser::OnUpdate(TimeStep ts) {
	for(auto& page : s_Pages) {
		auto gen = s_DLLs[page.Name];
		auto get = gen->GetFunction<UIObject*, std::string>("GetObject");

		page.Traverse(
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
	}

	if(s_CurrentPage)
		s_CurrentPage->Render();
}

void UIBrowser::SetPage(const std::string& name) {
	s_CurrentPage = GetPage(name);
}

UIPage* UIBrowser::GetPage(const std::string& name) {
	if(name == "")
		return nullptr;

	for(auto& page : s_Pages)
		if(page.Name == name)
			return &page;

	return nullptr;
}

}