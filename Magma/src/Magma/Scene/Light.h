#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace Magma {

struct Light {
	glm::vec3 Position;
	glm::vec3 Ambient;
	glm::vec3 Diffuse;
	glm::vec3 Specular;
};

struct DirectionalLight : public Light {
	glm::vec3 Direction;
};

struct PointLight : public Light {
	float Constant;
	float Linear;
	float Quadratic;
};

struct SpotLight : public Light {
	glm::vec3 Direction;

	float CutoffAngle;
	float OuterCutoffAngle;
};

}