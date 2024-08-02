#include "PhysicsSystem.h"

namespace Magma {

struct Collide { };

void RegisterForCollision(Entity& e1, Entity& e2) {
	if(!e1.Has<RigidBodyComponent>() || !e2.Has<RigidBodyComponent>()) {
		
	}
}

bool Collides(Entity& e1, Entity& e2) {

}


}