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
	static void EditorLoad(UIPage& page, const std::string& path);
	static void EditorSave(const UIPage& page, const std::string& path);
	static Map<UIElementType, ThemeElement> LoadTheme(const std::string& path);

	static void RuntimeSave(const UIPage& page, const std::string& projectPath,
							const std::string& exportPath);
	static Ref<ScriptModule> RuntimeLoad(UIPage& page,
										 const std::string& projectPath);
};

}