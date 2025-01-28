#pragma once

#include <Magma/UI/UIPage.h>

#include "Editor/Panel.h"

namespace Magma {

class UIElementEditorPanel : public Panel {
public:
	UIElementEditorPanel(UI::UIPage* page);
	~UIElementEditorPanel() = default;

	void Update(TimeStep ts) override;
	void Draw() override;

	void SetContext(UI::UIPage* page);

	void Select(UI::UIElement* element) {
		m_Selected = element;
	}

private:
	UI::UIPage* m_Context;
	UI::UIElement* m_Selected = nullptr;
};

}