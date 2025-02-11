#pragma once

#include "Core/Defines.h"

namespace VolcaniCore {

class Cubemap {
public:
	Cubemap(const std::string& cubemap_folder) { }
	Cubemap(const std::vector<std::string>& faces) { }

	static Ref<Cubemap> Create(const std::string& cubemap_folder);
	static Ref<Cubemap> Create(const std::vector<std::string>& faces);

	template<typename Derived>
	Derived* As() const { return ((Derived*)(this)); }
};

}