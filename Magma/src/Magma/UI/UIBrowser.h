#pragma once

#include "UIPage.h"

namespace Magma::UI {

class UIBrowser {
public:
	static void Render();

	static void Load(const std::string& folderPath);
	static void SetPage(const std::string& name);

private:
	static List<UIPage> s_Pages;
	static UIPage* s_CurrentPage;
};

}