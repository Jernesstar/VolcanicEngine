#pragma once

#include "Editor/Tab.h"
#include "Editor/UILoader.h"

#include "UI/UIPage.h"

using namespace VolcaniCore;

namespace Magma {

class UITab : public Tab {
public:
	static Theme& GetTheme();

public:
	UITab();
	UITab(const std::string& path);
	~UITab();

	void Update(TimeStep ts) override;
	void Render() override;

	void Load(const std::string& path);
	void Save(const std::string& path);

	void NewUI();
	void OpenUI();
	void SaveUI();

	void AddWindow();
	void AddButton();
	void AddImage();

private:
	UI::UIPage m_Root;
	Theme m_Theme;
	std::string m_UIPath;

private:
	void Setup();
};

}