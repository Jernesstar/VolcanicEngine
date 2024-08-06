#pragma once

#include <Magma/ECS/Entity.h>

namespace TheMazeIsLava {

class Player : public Entity {
public:
	Player();
	~Player() = default;
};

}