#pragma once

#include <string>

namespace Magma {

class Screen {
public:
	const std::string Name;
	const std::string Scene;
	const std::string Page;

public:
	Screen(const std::string& name,
			const std::string& scene, const std::string& page)
		: Name(name), Scene(scene), Page(page) { }
	~Screen() = default;
};

}