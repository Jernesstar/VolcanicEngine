#pragma once

#include <glm/vec3.hpp>

namespace Ash {

float Lerp(float a, float b, float t) {
	return a + (1 - t)*b;
}

}