#pragma once

#include "Core/Defines.h"

namespace VolcaniCore {

class Cubemap {
public:
	Cubemap(const std::string& cubemap_folder);
	Cubemap(const std::vector<std::string>& faces);

	inline static Ref<Cubemap> Create(const std::string& cubemap_folder);
	inline static Ref<Cubemap> Create(const std::vector<std::string>& faces);
};

}