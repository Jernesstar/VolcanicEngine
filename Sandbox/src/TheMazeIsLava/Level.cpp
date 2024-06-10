#include "Level.h"

#include <Renderer/Renderer.h>
#include <Renderer/Transform.h>

#include <OpenGL/Renderer.h>

namespace TheMazeIsLava {

void Level::Init() {
	s_Lava  = Texture::Create("Sandbox/assets/images/lava.png");
	s_Stone = Texture::Create("Sandbox/assets/images/stone.png");
	// s_Staircase  = Texture::Create("Sandbox/assets/images/staircase.png");
}

Level::Level(std::vector<std::vector<uint32_t>> map, const std::function<float(float t)>& lavaSpeed)
	: m_TileMap(map), LavaSpeed(lavaSpeed), m_Width(map[0].size()), m_Height(map.size())
{
	for(uint32_t y = 0; y < m_Height; y++) {
		for(uint32_t x = 0; x < m_Width; x++) {
			if(map[y][x] == 3)
				m_Goal = { x, y };
		}
	}
}

Level::~Level() {

}

void Level::Render(TimeStep ts) {
	m_TimeStep = ts;
	m_TimeSinceLevelStart += (float)ts;
	PropagateLava();

	DrawStoneBlock();
}

void Level::PropagateLava() {
	uint32_t lavaSpeed = LavaSpeed(m_TimeSinceLevelStart);

	// Propagate the lava in tile space, flow animation interpolates smoothly
}

void Level::DrawStoneBlock() {
	Application::GetRenderer()->As<OpenGL::Renderer>()->DrawCube(s_Stone, Transform{ });
}

}