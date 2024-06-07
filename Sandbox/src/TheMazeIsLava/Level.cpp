#include "Level.h"

#include <Renderer/Renderer.h>
#include <Renderer/Transform.h>

namespace TheMazeIsLava {

void Level::Init() {
	// TODO: Procedurally create lava and have it spread naturally; 3D Stone blocks
	s_Lava  = Texture::Create("Sandbox/assets/images/lava.png");
	s_Stone = Texture::Create("Sandbox/assets/images/stone.png");
	s_Door  = Texture::Create("Sandbox/assets/images/door.png");
}

Level Level::Create(const std::string& levelPath) {

}

Level::Level(std::vector<std::vector<uint32_t>> map,
	const std::function<float(float timeSinceLevelStart)>& lavaSpeed)
	: m_TileMap(map), LavaSpeed(lavaSpeed), m_Width(map[0].size()), m_Height(map.size())
{
	for(uint32_t y = 0; y < m_Height; y++) {
		for(uint32_t x = 0; x < m_Width; x++) {
			if(map[y][x] == 3)
				m_Goal = { x, y };
		}
	}
}

void Level::Render(TimeStep ts) {
	m_TimeStep = ts;
	m_TimeSinceLevelStart += (float)ts;
	PropagateLava();

	for(uint32_t y = 0; y < m_Height; y++) {
		for(uint32_t x = 0; x < m_Width; x++) {
			if(m_TileMap[y][x] == 0)
				Renderer::RenderTexture(s_Stone, Transform{ .Translation = { x, y, 0.0f } });
		}
	}
}

void Level::PropagateLava() {
	uint32_t lavaSpeed = LavaSpeed(m_TimeStep);
}

}