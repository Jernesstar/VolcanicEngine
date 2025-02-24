#pragma once

#include <Magma/Core/Project.h>

#include <Magma/UI/UIPage.h>

#include <Magma/Script/ScriptModule.h>

using namespace Magma;
using namespace Magma::Script;
using namespace Magma::UI;

namespace Magma {

struct ThemeElement {
	uint32_t Width = 0;
	uint32_t Height = 0;
	int32_t x = 0, y = 0; // Could be more accurately named xOffset and yOffset
	XAlignment xAlignment;
	YAlignment yAlignment;
	glm::vec4 Color = glm::vec4(0.0f);
	glm::vec4 TextColor = glm::vec4(0.0f);
	Ref<Texture> Image = nullptr;
	uint32_t BorderWidth = 0;
	uint32_t BorderHeight = 0;
	glm::vec4 BorderColor = glm::vec4(0.0f);
	Ref<Texture> BorderImage = nullptr;
};

using Theme = Map<UIElementType, ThemeElement>;

class UILoader {
public:
	static void EditorLoad(UIPage& page, const std::string& path,
							const Theme& theme);
	static void EditorSave(const UIPage& page, const std::string& path);
	static Map<UIElementType, ThemeElement> LoadTheme(const std::string& path);

	static void RuntimeSave(const UIPage& page, const std::string& projectPath,
							const std::string& exportPath);
};

}