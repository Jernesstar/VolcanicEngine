#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace VolcaniCore {

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec4 TexCoord_Color; // if the alpha channel is 0, used as tex coord
};

}