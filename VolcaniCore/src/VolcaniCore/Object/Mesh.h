#pragma once

#include <vector>
#include <memory>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "Object/Texture.h"

namespace VolcaniCore {

class Mesh {
public:
	Mesh() = default;
	virtual ~Mesh() = default;

	Ref<Mesh> Create();
};

}