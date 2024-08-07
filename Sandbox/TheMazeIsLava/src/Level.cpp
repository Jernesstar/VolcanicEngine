#include "Level.h"

#include <Renderer/Renderer3D.h>

#include <Magma/ECS/EntityBuilder.h>

#include "GameState.h"

using namespace VolcaniCore;
using namespace Magma::ECS;

namespace TheMazeIsLava {

Level::Level(const std::string& name, std::vector<std::vector<uint32_t>> map)
	: Name(name), m_Tilemap(map), m_Width(map[0].size()), m_Height(map.size())
{
	TraverseTilemap(
	[this](uint32_t x, uint32_t y) {
		if(IsLava(x, y))
			m_LavaPoints.push_back({ x, y });
	});
}

Level::~Level() {

}

void Level::Run(TimeStep ts) {
	m_TimeStep = ts;
	m_TimeSinceLevelStart += (float)ts;

	PropagateLava();
}

void Level::PropagateLava() {
	// uint32_t lavaSpeed = LavaSpeed(m_TimeSinceLevelStart);

	// Propagate the lava in tile space, flow animation interpolates smoothly
}

Ref<Scene> Level::Load() {
	Ref<Scene> scene = CreateRef<Scene>();
	auto& world = scene->GetEntityWorld();

	TraverseTilemap(
	[&](uint32_t x, uint32_t y) {
		ECS::Entity floor = ECS::EntityBuilder(world)
		.Add<TransformComponent>(Transform{ .Translation = { x, 0.0f, y } })
		.Add<MeshComponent>(GameState::Wall)
		.Finalize();

		if(!IsWall(x, y)) return;

		ECS::Entity wall = ECS::EntityBuilder(world)
		.Add<TransformComponent>(Transform{ .Translation = { x, 1.0f, y } })
		.Add<MeshComponent>(GameState::Wall)
		.Finalize();

		if(!IsGoal(x, y)) return;

		// TODO: Staircase model
		ECS::Entity stairs = ECS::EntityBuilder(world)
		.Add<TransformComponent>(Transform{ .Translation = { x, 1.0f, y } })
		// .Add<MeshComponent>(GameState::StairModel)
		.Finalize();
	});

	return scene;
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