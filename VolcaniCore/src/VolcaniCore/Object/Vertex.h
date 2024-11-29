#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
// #include <glm/vec4.hpp>

namespace VolcaniCore {

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoord;
};

}