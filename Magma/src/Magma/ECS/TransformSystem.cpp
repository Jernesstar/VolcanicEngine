#include "TransformSystem.h"

namespace Magma::ECS {

Transform TransformSystem::GetTransform(Entity& entity) {
	auto& tc = entity.Get<TransformComponent>();
	return Transform{ tc.Translation, tc.Rotation, tc.Scale };
}

void TransformSystem::SetTransform(Entity& entity, const Transform& tr) {
	entity.Get<TransformComponent>() = tr;
}

void TransformSystem::Translate(Entity& entity) {

}

void TransformSystem::Rotate(Entity& entity) {

}

void TransformSystem::Scale(Entity& entity) {

}

}