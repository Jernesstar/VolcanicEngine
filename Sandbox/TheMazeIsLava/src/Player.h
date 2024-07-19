#pragma once

#include <Object/Model.h>

namespace TheMazeIsLava {

class Player : public Entity {
public:
	Player();
	~Player();

	// NOTE: InputProvider ?
	void Move();
	void Render();

private:
	float x, y;
	// Ref<Model> m_Model;
};

}