#pragma once

#include "Panel.h"

#include "UI/UI.h"

namespace Magma {

class UIVisualizerPanel : public Panel {
public:
	UIVisualizerPanel(Ref<UI::UIElement> scene);
	~UIVisualizerPanel() = default;

	void SetContext(Ref<UI::UIElement> scene);

	void Update(TimeStep ts) override;
	void Draw() override;

private:

};

}