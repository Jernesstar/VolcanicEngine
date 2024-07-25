#pragma once

#include <flecs.h>

#include "Entity.h"

namespace Magma {

class EntitySystem {
public:
	EntitySystem() = default;
	~EntitySystem() = default;

	flecs::world& GetWorld() { return m_World; }

private:
	flecs::world m_World;
};

}