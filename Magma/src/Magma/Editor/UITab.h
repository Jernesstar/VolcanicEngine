#pragma once

#include "Tab.h"

#include "UI/UIElement.h"

using namespace VolcaniCore;

namespace Magma {

class UITab : public Tab {
public:
	UITab();
	UITab(Ref<UI::UIElement> scene);
	~UITab();

	void Update(TimeStep ts) override;
	void Render() override;

	void SetUI(Ref<UI::UIElement> element);
	void SetUI(const std::string& path);

private:
	Ref<UI::UIElement> m_Root;

private:
	void Setup();

	void NewUI();
	void OpenUI();
	void SaveUI();

	void AddWindow();
	void AddButton();
	void AddImage();
};

}