#pragma once

#include "UI/UIPage.h"
#include "Scene/Scene.h"

namespace Magma {

class Screen {
public:
	const std::string Name;

public:
	Screen(const std::string& name, const UI::UIPage& page, const Scene& scene)
		: Name(name), m_Page(page), m_Scene(scene) { }
	~Screen() = default;

	UI::UIPage& GetPage() { return m_Page; }
	Scene& GetScene() { return m_Scene; }

private:
	UI::UIPage m_Page;
	Scene m_Scene;
};

}