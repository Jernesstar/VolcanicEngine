#pragma once

#include "Tab.h"

#include "UI/UIElement.h"

using namespace VolcaniCore;

namespace Magma {

class UITab : public Tab {
public:
	UITab(Ref<UI::UIElement> scene);
	~UITab();

	void Update(TimeStep ts) override;
	void Render() override;

private:
	Ref<UI::UIElement> m_Root;

	void NewUI();
	void OpenUI();
	void SaveUI();

	void AddWindow();
	void AddButton();
	// void AddImage();
};

}