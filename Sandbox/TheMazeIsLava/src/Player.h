#pragma once

#include <Magma/ECS/Entity.h>
#include <Magma/ECS/World.h>

using namespace Magma;
using namespace Magma::ECS;

namespace TheMazeIsLava {

class Player : public Entity {
public:
	Player(ECS::World& world);
	~Player() = default;
};

}