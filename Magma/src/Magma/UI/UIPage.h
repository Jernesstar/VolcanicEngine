#pragma once

#include <VolcaniCore/Core/Time.h>
#include <VolcaniCore/Core/List.h>

#include "Core/DLL.h"

#include "UIElement.h"
#include "Window.h"
#include "Button.h"
#include "Dropdown.h"
#include "Text.h"
#include "TextInput.h"
#include "Image.h"

using namespace VolcaniCore;

namespace Magma::UI {

enum class TraversalStage { Begin, End };

class UIPage {
public:
	bool Visible = true;
	std::string Name;

	List<Window> Windows;
	List<Button> Buttons;
	List<Dropdown> Dropdowns;
	List<Text> Texts;
	List<TextInput> TextInputs;
	List<Image> Images;

	Map<std::string, List<UINode>> LayerNodes;

public:
	UIPage();
	UIPage(const std::string &name);
	UIPage(const UIPage& other) {
		*this = other;
	}
	~UIPage() = default;

	UIPage& operator =(const UIPage& other);

	void Render();
	void SetLayer(const std::string& name = "Root");
	void PushLayer(const std::string& name);
	void PopLayer();

	void Traverse(const Func<void, UIElement*>& func, bool dfs = true);
	void Traverse(const Func<void, UIElement*, TraversalStage>& func);

	UINode Add(UIElementType type, const std::string& id);
	void Add(const UINode& node, const std::string& layer = "Root");

	void Delete(const UINode& node);
	void Delete(const std::string& id);

	void Clear();
	List<UIElement*> GetFirstOrderElements(const std::string& layer = "Root") const;

	UIElement* Get(const UINode& node) const;
	UIElement* Get(const std::string& id) const;

private:
	List<std::string> m_Layers;
};

}