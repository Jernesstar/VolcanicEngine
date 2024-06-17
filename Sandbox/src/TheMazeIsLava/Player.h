#include "Player.h"

#include <Core/Input.h>

using namespace VolcaniCore;

namespace TheMazeIsLava {


Player::Player() {
	
}

Player::~Player() {
	
}

void Player::Move() {
	x += 5 * ((Input::KeyPressed(Key::Right) || Input::KeyPressed(Key::A)) - (Input::KeyPressed(Key::Left) || Input::KeyPressed(Key::D)));
	y += 5 * ((Input::KeyPressed(Key::Up) || Input::KeyPressed(Key::W)) - (Input::KeyPressed(Key::Down) || Input::KeyPressed(Key::S)));
}

void Player::Render() {
	Renderer::RenderModel(m_Model, Transform{ x, 0.0f, y });
}


}