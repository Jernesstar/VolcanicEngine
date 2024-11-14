#include "TransformSystem.h"

namespace Magma::ECS {

Transform TransformSystem::GetTransform(Entity& entity) {
	auto& tc = entity.Get<TransformComponent>();
	return Transform{ tc.Translation, tc.Rotation, tc.Scale };
}

void TransformSystem::SetTransform(Entity& entity, const Transform& tr) {
	entity.Get<TransformComponent>() = tr;
}

void TransformSystem::Translate(Entity& entity, const glm::vec3& vec) {
	auto& [tc] = GetRequired(entity);
}

void TransformSystem::Rotate(Entity& entity, const glm::vec3& vec) {

}

void TransformSystem::Scale(Entity& entity, const glm::vec3& vec) {

}

}