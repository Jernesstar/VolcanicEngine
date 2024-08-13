#include "TransformSystem.h"

namespace Magma::ECS {

Transform TransformSystem::GetTransform(Entity& entity) {
	if(!entity.Has<TransformComponent>())
		return;

	auto& t = entity.Get<TransformComponent>();
	return Transform{
		.Translation = t.Translation,
		.Rotation	 = t.Rotation,
		.Scale		 = t.Scale
	};
}

void TransformSystem::SetTransform(Entity& entity, const Transform& tr) {

}

void TransformSystem::Translate(Entity& entity) {

}

void TransformSystem::Rotate(Entity& entity) {

}

void TransformSystem::Scale(Entity& entity) {

}

}