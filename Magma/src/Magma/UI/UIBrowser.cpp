#include "UIBrowser.h"

#include <filesystem>

#include <VolcaniCore/Core/Log.h>
#include <VolcaniCore/Core/FileUtils.h>

namespace Magma::UI {

static List<UIPage> s_Pages;
static UIPage* s_CurrentPage;

void UIBrowser::OnUpdate(TimeStep ts) {
	for(auto& page : s_Pages)
		page.Update(ts);
}

void UIBrowser::OnRender() {
	s_CurrentPage->Render();
}

void UIBrowser::Load(const std::string& folderPath) {
	namespace fs = std::filesystem;

	for(auto filePath : FileUtils::GetFiles(folderPath, { ".json" })) {
		fs::path p(filePath);
		auto filePathName = folderPath / p.stem().stem().stem();
		s_Pages.emplace_back(filePathName.string());
	}

	s_CurrentPage = &s_Pages[0];
}

void UIBrowser::Reload() {
	for(auto& page : s_Pages)
		page.Reload();
}

void UIBrowser::SetPage(const std::string& name) {
	s_CurrentPage = GetPage(name);

	// if(s_CurrentPage) {
	// 	namespace fs = std::filesystem;
	// 	fs::path p(s_CurrentPage->GetPath());
	// 	auto path = p.parent_path() / name;

	// 	s_CurrentPage = &s_Pages.emplace_back(path.string() + ".magma.ui.json");
	// }
}

UIPage* UIBrowser::GetPage(const std::string& name) {
	if(name == "")
		return nullptr;

	for(auto& page : s_Pages)
		if(page.GetName() == name)
			return &page;

	return nullptr;
}

}