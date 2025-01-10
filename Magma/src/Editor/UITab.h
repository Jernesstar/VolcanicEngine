#pragma once

#include "Tab.h"

#include "UI/UIPage.h"

using namespace VolcaniCore;

namespace Magma {

class UITab : public Tab {
public:
	UITab();
	UITab(const std::string& path);
	UITab(const UI::UIPage& page);
	~UITab();

	void Update(TimeStep ts) override;
	void Render() override;

	void SetUI(const std::string& path);

private:
	UI::UIPage m_Root;

private:
	void Setup();
	void Save(const std::string& path);

	void NewUI();
	void OpenUI();
	void SaveUI();

	void AddWindow();
	void AddButton();
	void AddImage();
};

}