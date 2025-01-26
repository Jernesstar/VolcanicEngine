#pragma once

#include <Magma/Core/Project.h>
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
	static Ref<DLL> GetDLL(const std::string& pageName);

	static List<UIPage> LoadFolder(const Project& projectx);
	static void CompileFolder(const std::string& folderPath);
};

}