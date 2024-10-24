#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace VolcaniCore {

using Vec2f = glm::vec2;
using Vec3f = glm::vec3;
using Vec2i = glm::ivec2;
using Vec3i = glm::ivec3;
using Vec2 = Vec2f;
using Vec3 = Vec3f;

constexpr float PI = 3.14159265358979328462f;
// constexpr float E  = 2.71828182859f;

static constexpr float RadToDeg(float radians) {
	return (180.0f / PI) * radians;
}
static constexpr float DegToRad(float degrees) {
	return (PI / 180.0f) * degrees;
}

}