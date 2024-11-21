#pragma once

#include <VolcaniCore/Core/Buffer.h>

#include "UI.h"

namespace Magma::UI {

using UINode = std::pair<UIElement::Type, std::string>;

class UIRoot {
public:
	UIRoot();
	~UIRoot();

	void Render();

private:
	Buffer<Window> Windows;
	Buffer<Button> Buttons;
	Buffer<Dropdown> Dropdowns;
	Buffer<Text> Texts;
	Buffer<TextInput> TextInputs;
	Buffer<Image> Images;

	List<UINode> m_Nodes;
	List<uint32_t> m_Orders;

private:
	void Render(UINode& node);
	void Reallocate();
};

}