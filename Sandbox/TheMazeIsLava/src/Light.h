#pragma once

namespace TheMazeIsLava {

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

// struct SpotLight : public Light, public DirectionalLight {
// 	float CutoffAngle;
// 	float OuterCutoffAngle;
// };

}