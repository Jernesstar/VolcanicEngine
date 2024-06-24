#pragma once

#include <Renderer/Model.h>

namespace TheMazeIsLava {

class Player {
public:
	Player();
	~Player();

	// NOTE: InputProvider ?
	void Move();
	void Render();

private:
	float x, y;
	Ref<Model> m_Model;
};

}