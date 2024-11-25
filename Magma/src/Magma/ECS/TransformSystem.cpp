#include "TransformSystem.h"

namespace Magma::ECS {

Transform TransformSystem::GetTransform(Entity& entity) {
	auto [tc] = GetRequired(entity);
	return { tc.Translation, tc.Rotation, tc.Scale };
}

void TransformSystem::SetTransform(Entity& entity, const Transform& tr) {
	entity.Get<TransformComponent>() = tr;
}

void TransformSystem::Translate(Entity& entity, const glm::vec3& vec) {
	auto [tc] = GetRequired(entity);
	tc.Translation += vec;
}

void TransformSystem::Rotate(Entity& entity, const glm::vec3& vec) {
	auto [tc] = GetRequired(entity);
	tc.Rotation = glm::rotate(glm::quat(vec), tc.Rotation);
}

void TransformSystem::Scale(Entity& entity, const glm::vec3& vec) {
	auto [tc] = GetRequired(entity);
	tc.Scale = tc.Scale * vec;
}

}