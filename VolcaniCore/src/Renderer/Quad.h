#pragma once

#include <glm/vec3.hpp>

namespace VolcaniCore {

class Quad {
public:
	Quad(glm::vec3 color);

	static Ref<Quad> Create(glm::vec3 color);
};

}