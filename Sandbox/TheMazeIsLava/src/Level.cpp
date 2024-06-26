#include "Level.h"

#include <Renderer/Renderer.h>
#include <Renderer/Transform.h>

#include <OpenGL/Renderer.h>

namespace TheMazeIsLava {

Level::Level(const std::string& name, std::vector<std::vector<uint32_t>> map)
	: Name(name), m_Tilemap(map), m_Width(map[0].size()), m_Height(map.size())
{
	TraverseTilemap(
	[](uint32_t x, uint32_t y) {
		if(IsGoal(x, y))
			m_Goal = { x, m_Height - y };
		if(IsLave(x, y))
			m_LavaPoints.push_back({ x, y });
	});
}

Level::~Level() {

}

void Level::Render(TimeStep ts) {
	m_TimeStep = ts;
	m_TimeSinceLevelStart += (float)ts;

	DrawStoneBlocks();
	PropagateLava(); // Fluid simulation
}

void Level::PropagateLava() {
	// uint32_t lavaSpeed = LavaSpeed(m_TimeSinceLevelStart);

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

void Level::TraverseTilemap(
	const std::function<void(uint32_t x, uint32_t y)>& func)
{
	for(uint32_t i = 0; i < m_Height; i++)
		for(uint32_t j = 0; j < m_Width; j++)
			func(j, i);
}

bool Level::IsCheckpoint(uint32_t col, uint32_t row) {
	return m_Tilemap[row][col] == 1; // TODO: New numbering system
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
	if(IsPath(col, row) || IsLava(col, row) || IsEnd(col, row)) {
		return false;
	}

	var result = false;
	if(col - 1 >= 0)                              // Left
		result |= (m_Tilemap[row][col - 1] == 1);
	if(col + 1 < m_Tilemap.size())                // Right
		result |= (m_Tilemap[row][col + 1] == 1);
	if(row - 1 >= 0)                              // Up
		result |= (m_Tilemap[row - 1][col] == 1);
	if(row + 1 < m_Tilemap.size())                // Down
		result |= (m_Tilemap[row + 1][col] == 1);

	return result;
}

}