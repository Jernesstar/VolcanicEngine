#pragma once

#include "UIPage.h"

namespace Magma::UI {

class UIBrowser {
public:
	static void Render();

	static void Load(const std::string& folderPath);
	static void SetPage(const std::string& name);
};

}