#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

namespace VolcaniCore {

struct Transform {
	glm::vec3 Translation = { 0, 0, 0 };
	glm::vec3 Rotation = { 0, 0, 0 };
	glm::vec3 Scale = { 1, 1, 1 };

	glm::mat4 GetTransform()
	{
		return glm::translate(glm::mat4(1.0f), Translation)
			 * glm::toMat4(glm::quat(Rotation))
			 * glm::scale(glm::mat4(1.0f), Scale);
	}
};

}