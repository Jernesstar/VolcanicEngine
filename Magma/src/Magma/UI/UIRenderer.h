#pragma once

#include "Window.h"
#include "Button.h"
#include "Image.h"
#include "Text.h"
#include "TextInput.h"
#include "Dropdown.h"

namespace Magma::UI {

class UIRenderer {
public:
	static void Init();
	static void Close();
	static void BeginFrame();
	static void EndFrame();

	static UIState DrawWindow(Window& window);
	static UIState DrawButton(Button& button);
	static UIState DrawImage(Image& image);
	static UIState DrawText(Text& text);
	static UIState DrawTextInput(TextInput& textInput);
	static UIState DrawDropdown(Dropdown& dropdown);

	static UIState DrawMenuBar(const std::string& name);
	static UIState DrawMenu(const std::string& name);

	static UIState DrawTabBar(const std::string& name);
	static UIState DrawTab(const std::string& name);

	static void ShowPopupLabel(const std::string& str);
};

}