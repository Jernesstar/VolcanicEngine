#pragma once

#include "Panel.h"

#include "UI/UIPage.h"

namespace Magma {

class UIVisualizerPanel : public Panel {
public:
	UIVisualizerPanel(UI::UIPage* page);
	~UIVisualizerPanel();

	void SetContext(UI::UIPage* page);

	void Update(TimeStep ts) override;
	void Draw() override;

private:
	UI::UIPage* m_Context;
	UI::UIPage* m_Running;
	UI::UINode m_Node;
};

}