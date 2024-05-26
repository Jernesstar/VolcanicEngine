#pragma once

#include <vector>
#include <memory>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "Renderer/Texture.h"

namespace VolcaniCore {

class Model {
public:
	static Ref<Model> Create(const std::string& path);

	template<typename Derived>
	Derived* As() const { return (Derived*)(this); }
};

}