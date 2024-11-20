#pragma once

#include "Panel.h"

#include "UI/UI.h"

namespace Magma {

class UIElementPickerPanel : public Panel {
public:
	UIElementPickerPanel(Ref<UI::UIElement> scene);
	~UIElementPickerPanel() = default;

	void SetContext(Ref<UI::UIElement> scene);

	void Update(TimeStep ts) override;
	void Draw() override;

private:

};

}