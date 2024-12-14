#include "UIBrowser.h"

#include <filesystem>

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
	for(auto filePath : FileUtils::GetFiles(folderPath, { ".magma.ui.json" }))
		s_Pages.emplace_back(filePath);

	s_CurrentPage = &s_Pages[0];
}

void UIBrowser::SetPage(const std::string& name) {
	if(name == "")
		return;

	for(auto& page : s_Pages)
		if(page.GetName() == name) {
			s_CurrentPage = &page;
			return;
		}

	if(s_CurrentPage) {
		namespace fs = std::filesystem;
		fs::path p(s_CurrentPage->GetPath());
		auto path = p.parent_path() / name;

		s_CurrentPage = &s_Pages.emplace_back(path.string() + ".magma.ui.json");
	}
}

}