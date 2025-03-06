#pragma once

#include <string>

namespace Magma {

class Screen {
public:
	const std::string Name;
	const std::string Scene;
	const std::string UI;

public:
	Screen(const std::string& name,
			const std::string& scene, const std::string& ui)
		: Name(name), Scene(scene), UI(ui) { }
	~Screen() = default;
};

}