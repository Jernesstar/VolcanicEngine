#pragma once

#include <glm/vec4.hpp>

namespace Magma {

class Light {
public:
	glm::vec4 Color;

public:
	Light(const glm::vec4& color) : m_Color(color) { }
	~Light();
 };

}