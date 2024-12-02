#include "UIBrowser.h"

#include <filesystem>

#include <VolcaniCore/Core/FileUtils.h>

namespace Magma::UI {

static void CompilePage(const std::string& funcPath);

void UIBrowser::Render() {
	s_CurrentPage->Render();
}

void UIBrowser::Load(const std::string& folderPath) {
	for(auto filePath : FileUtils::GetFiles(folderPath, { ".magma.ui.json" })) {
		s_Pages.emplace_back(filePath);
		std::filesystem::path file(filePath);
		CompilePage(file.stem().string() + ".magma.ui.func");
	}
}

void UIBrowser::SetPage(const std::string& name) {
	for(auto& page : s_Pages) {
		if(page.GetName() == name)
			s_CurrentPage = &page;
	}
}

void CompilePage(const std::string& funcPath) {
	// 
}

}