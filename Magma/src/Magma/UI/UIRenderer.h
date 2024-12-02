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

	static void DrawWindow(Window& window);
	static void DrawButton(Button& button);
	static void DrawImage(Image& image);
	static void DrawText(Text& text);
	static void DrawTextInput(TextInput& textInput);
	static void DrawDropdown(Dropdown& dropdown);

	static void DrawMenuBar(const std::string& name);
	// static UIState DrawMenuBar(const std::string& name);
	static void DrawMenu(const std::string& name);

	static void DrawTabBar(const std::string& name);
	static void DrawTab(const std::string& name);
};

}