#pragma once

#include "UI/UIPage.h"
#include "Scene/Scene.h"

namespace Magma {

class Screen {
public:
	const std::string Name;
	const std::string Page;
	const std::string Scene;

public:
	Screen(const std::string& name,
			const std::string& page, const std::string& scene)
		: Name(name), Page(page), Scene(scene) { }
	~Screen() = default;
};

}