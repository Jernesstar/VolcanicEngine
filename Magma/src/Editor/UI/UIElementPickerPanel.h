#pragma once

#include <Magma/UI/UIPage.h>

#include "Editor/Panel.h"

namespace Magma {

class UIElementPickerPanel : public Panel {
public:
	UIElementPickerPanel(UI::UIPage* page);
	~UIElementPickerPanel() = default;

	void SetContext(UI::UIPage* page);

	void Update(TimeStep ts) override;
	void Draw() override;

private:
	UI::UIPage* m_Context;
};

}