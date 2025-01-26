#pragma once

#include <Magma/UI/UIPage.h>

#include "Editor/Panel.h"

namespace Magma {

class UIHierarchyPanel : public Panel {
public:
	UIHierarchyPanel(UI::UIPage* page);
	~UIHierarchyPanel() = default;

	void SetContext(UI::UIPage* page);
	void SetLayer(uint32_t layerIndex);

	void Update(TimeStep ts) override;
	void Draw() override;

private:
	UI::UIPage* m_Context;
};

}