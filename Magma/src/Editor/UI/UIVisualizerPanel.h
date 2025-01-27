#pragma once

#include <Magma/UI/UIPage.h>

#include "Editor/Panel.h"

namespace Magma {

class UIVisualizerPanel : public Panel {
public:
	UIVisualizerPanel(UI::UIPage* page);
	~UIVisualizerPanel();

	void Update(TimeStep ts) override;
	void Draw() override;

	void SetContext(UI::UIPage* page);
	void Select(UI::UIElement* element) {
		m_Selected = element;
	}

private:
	UI::UIPage* m_Context;
	UI::UIPage* m_Running;
	UI::UINode m_RootNode;
	UI::UIElement* m_Selected;
};

}