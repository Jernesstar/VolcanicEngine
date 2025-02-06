#pragma once

#include <Magma/Core/Project.h>

#include <Magma/UI/UIPage.h>

#include <Magma/Script/ScriptModule.h>

using namespace Magma;
using namespace Magma::Script;
using namespace Magma::UI;

namespace Lava {

class UILoader {
public:
	static void Load(UIPage& page, const std::string& path);
	static void Save(const UIPage& page, const std::string& path);
	static Map<UIElementType, ThemeElement> LoadTheme(const std::string& path);

	static void Compile(const std::string& path);
	static Ref<ScriptModule> GetModule(const std::string& name);
};

}