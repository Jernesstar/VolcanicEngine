#include "Game.h"

namespace TheMazeIsLava {

Game::Game() { 
	Reset();
}

void Game::OnUpdate(TimeStep ts) {

}

void Game::Reset() {
	m_Levels.push_back(
	Level {
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		
	});
}

void Game::Play() {

}

void Game::Pause() {

}

}