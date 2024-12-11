#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace VolcaniCore {

struct Point {
	glm::vec3 Position;
	glm::vec4 Color;
	float size;
};

}