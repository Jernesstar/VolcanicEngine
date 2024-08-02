#include "System.h"

namespace Magma {

struct Collide { };

void PhysicsSystem::RegisterForCollision(Entity& e1, Entity& e2) {
	if(!e1.Has<RigidBodyComponent>() || !e2.Has<RigidBodyComponent>()) {
		
	}
}

bool PhysicsSystem::Collides(Entity& e1, Entity& e2) {

}


}