#pragma once

#include <Magma/UI/UIPage.h>

using namespace Magma;
using namespace Magma::UI;

namespace Lava {

class UILoader {
public:
	static void Load(UIPage& page, const std::string& path);
	static void Save(const UIPage& page, const std::string& path);
	static Map<UIElementType, ThemeElement> LoadTheme(const std::string& path);
	static void Compile(const std::string& path);
};

}