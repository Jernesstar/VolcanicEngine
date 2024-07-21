#include "Level.h"

#include <Renderer/Renderer3D.h>

#include "GameState.h"

using namespace VolcaniCore;

namespace TheMazeIsLava {

Level::Level(const std::string& name, std::vector<std::vector<uint32_t>> map)
	: Name(name), m_Tilemap(map), m_Width(map[0].size()), m_Height(map.size())
{
	TraverseTilemap(
	[this](uint32_t x, uint32_t y) {
		if(IsGoal(x, y))
			m_Goal = { x, m_Height - y };
		if(IsLava(x, y))
			m_LavaPoints.push_back({ x, y });
	});
}

Level::~Level() {

}

void Level::Render(TimeStep ts) {
	m_TimeStep = ts;
	m_TimeSinceLevelStart += (float)ts;

	DrawStoneBlocks();

	// if(!m_Paused)
	// 	PropagateLava(); // Fluid simulation
	// else
	// 	PauseUI->Render();
}

void Level::PropagateLava() {
	// uint32_t lavaSpeed = LavaSpeed(m_TimeSinceLevelStart);

	// Propagate the lava in tile space, flow animation interpolates smoothly
}

void Level::DrawStoneBlocks() {
	TraverseTilemap(
	[this](uint32_t x, uint32_t y) {
		Renderer3D::DrawMesh(GameState::Wall,
			Transform{ .Translation = glm::vec3{ x, 0.0f, y } });

		if(!IsWall(x, y))
			return;
		Renderer3D::DrawMesh(GameState::Wall,
			Transform{ .Translation = glm::vec3{ x, 1.0f, y } });
	});
}

void Level::TraverseTilemap(
	const std::function<void(uint32_t x, uint32_t y)>& func)
{
	for(uint32_t i = 0; i < m_Height; i++)
		for(uint32_t j = 0; j < m_Width; j++)
			func(j, i);
}

bool Level::IsCheckpoint(uint32_t col, uint32_t row) {
	return m_Tilemap[row][col] == 4; // TODO: New numbering system
}

bool Level::IsPath(uint32_t col, uint32_t row) {
	return m_Tilemap[row][col] == 1;
}

bool Level::IsLava(uint32_t col, uint32_t row) {
	return m_Tilemap[row][col] == 2;
}

bool Level::IsGoal(uint32_t col, uint32_t row) {
	return m_Tilemap[row][col] == 3;
}

bool Level::IsWall(uint32_t col, uint32_t row) {
	return m_Tilemap[row][col] == 0;
}

}