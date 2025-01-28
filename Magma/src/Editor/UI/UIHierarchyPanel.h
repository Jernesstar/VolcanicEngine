#pragma once

#include <imgui/imgui_internal.h>

#include <Magma/UI/UIPage.h>

#include "Editor/Panel.h"

namespace Magma {

class UIHierarchyPanel : public Panel {
public:
	UIHierarchyPanel(UI::UIPage* page);
	~UIHierarchyPanel() = default;

	void Update(TimeStep ts) override;
	void Draw() override;

	void SetContext(UI::UIPage* page);
	void SetLayer(uint32_t layerIndex);
	void Select(UI::UIElement* element) {
		m_Selected = element;
	}

private:
	UI::UIPage* m_Context;
	UI::UIElement* m_Selected = nullptr;

	ImRect Traverse(UI::UIElement* element);
};

}