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
				m_Goal = { x, m_Height - y };
			if(map[y][x] == 2)
				m_LavaPoints.push_back({ x, y });
		}
	}
}

Level::~Level() {

}

void Level::Render(TimeStep ts) {
	m_TimeStep = ts;
	m_TimeSinceLevelStart += (float)ts;
	PropagateLava();

	DrawStoneBlocks();
}

void Level::TraverseTilemap(
	const std::function<void((uint32_t x, uint32_t y))>& func)
{
	for(uint32_t i = 0; i < m_Height; i++)
		for(uint32_t j = 0; j < m_Width; j++)
			func(j, i);
}

void Level::PropagateLava() {
	uint32_t lavaSpeed = LavaSpeed(m_TimeSinceLevelStart);

	// Propagate the lava in tile space, flow animation interpolates smoothly
}

void Level::DrawStoneBlocks() {
	TraverseTilemap(
	[](uint32_t x, uint32_t y){
		Application::GetRenderer()->As<OpenGL::Renderer>()->Draw3DCube(s_Stone,
		Transform{
			.Translation = glm::vec3{ x, 0.0f, y }
		});

		if(!IsWall(x, y))
			return;
		Application::GetRenderer()->As<OpenGL::Renderer>()->Draw3DCube(s_Stone,
		Transform{
			.Translation = glm::vec3{ x, 1.0f, y }
		});
	});
}

bool Level::IsPath(uint32_t col, uint32_t row) {
	return m_TileMap[row][col] == 1;
}

bool Level::IsLava(uint32_t col, uint32_t row) {
	return m_TileMap[row][col] == 2;
}

bool Level::IsGoal(uint32_t col, uint32_t row) {
	return m_TileMap[row][col] == 3;
}

bool Level::IsWall(uint32_t col, uint32_t row) {
	if(IsPath(col, row) || IsLava(col, row) || IsEnd(col, row)) {
		return false;
	}

	var result = false;
	if(col - 1 >= 0)                              // Left
		result |= (m_TimeMap[row][col - 1] == 1);
	if(col + 1 < m_TimeMap.size())                // Right
		result |= (m_TimeMap[row][col + 1] == 1);
	if(row - 1 >= 0)                              // Up
		result |= (m_TimeMap[row - 1][col] == 1);
	if(row + 1 < m_TimeMap.size())                // Down
		result |= (m_TimeMap[row + 1][col] == 1);

	return result;
}

}