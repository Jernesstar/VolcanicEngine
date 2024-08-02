#pragma once

#include "Entity.h"

namespace Magma {

class System {
public:
	System();
	~System();

	// static void SomeOperation(Entity& entity);

private:
	
};

// class PhysicsSystem : public System {
class PhysicsSystem {
public:
	// PhysicsSystem();
	// ~PhysicsSystem();

	static void RegisterForCollision(Entity& e1, Entity& e2);
	static bool Collides(Entity& e1, Entity& e2);

private:
	
};

}