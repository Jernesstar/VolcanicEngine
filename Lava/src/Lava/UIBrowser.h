#pragma once

#include <Magma/UI/UIPage.h>

using namespace Magma::UI;

namespace Lava {

class UIBrowser {
public:
	static void Load(const std::string& folderPath);
	static void Reload();

	static void OnUpdate(TimeStep ts);

	static void SetPage(const std::string& name);
	static UIPage* GetPage(const std::string& name);
};

}