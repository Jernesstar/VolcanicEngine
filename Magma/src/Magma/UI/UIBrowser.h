#pragma once

#include "UIPage.h"

namespace Magma::UI {

class UIBrowser {
public:
	static void OnUpdate(TimeStep ts);
	static void OnRender();

	static void Load(const std::string& folderPath);
	static void SetPage(const std::string& name);
	static UIPage* GetPage(const std::string& name);
};

}