#include "UIBrowser.h"

#include <filesystem>

#include <VolcaniCore/Core/FileUtils.h>

namespace Magma::UI {

static List<UIPage> s_Pages;
static UIPage* s_CurrentPage;

void UIBrowser::Render() {
	s_CurrentPage->Render();
}

void UIBrowser::Load(const std::string& folderPath) {
	for(auto filePath : FileUtils::GetFiles(folderPath, { ".magma.ui.json" }))
		s_Pages.emplace_back(filePath);

	s_CurrentPage = &s_Pages[0];
}

void UIBrowser::SetPage(const std::string& name) {
	for(auto& page : s_Pages)
		if(page.GetName() == name)
			s_CurrentPage = &page;
}

}