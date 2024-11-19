#pragma once

#include "EntityBuilder.h"

#include "ECS/World.h"

namespace Magma::ECS {

EntityBuilder::EntityBuilder(const Entity& entity) {
	m_Handle = entity.GetHandle();
}

EntityBuilder::EntityBuilder(World& world) {
	m_Handle = world.AddEntity().GetHandle();
}

EntityBuilder::EntityBuilder(World& world, VolcaniCore::UUID id) {
	m_Handle = world.AddEntity(id).GetHandle();
}

EntityBuilder::EntityBuilder(World& world, const std::string& tag) {
	m_Handle = world.AddEntity(tag).GetHandle();
}

}