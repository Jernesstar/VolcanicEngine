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
		if(!element)
			m_Selected = nullptr;
		else
			m_Selected = m_Running->Get(element->GetID());
	}
	void Add(UI::UIElement* element);

private:
	UI::UIPage* m_Context;
	UI::UIPage* m_Running;
	UI::UIElement* m_Selected = nullptr;
};

}