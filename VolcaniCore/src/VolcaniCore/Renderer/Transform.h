#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

namespace VolcaniCore {

class Transform {
public:
	Transform() = default;
	Transform(const glm::vec3& translation, const glm::vec3& rotation, const glm::vec3& scale)
		: Translation(translation), Rotation(rotation), Scale(scale) { }

	glm::mat4 GetTransform()
	{
		return glm::translate(glm::mat4(1.0f), Translation)
			 * glm::toMat4(glm::quat(Rotation))
			 * glm::scale(glm::mat4(1.0f), Scale);
	}

public:
	glm::vec3 Translation = { 0, 0, 0 };
	glm::vec3 Rotation = { 0, 0, 0 };
	glm::vec3 Scale = { 1, 1, 1 };
};

}