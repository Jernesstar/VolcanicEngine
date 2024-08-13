#include "Level.h"

#include <Renderer/Renderer3D.h>

#include <Magma/ECS/EntityBuilder.h>

#include "GameState.h"

using namespace VolcaniCore;
using namespace Magma::ECS;

namespace TheMazeIsLava {

Level::Level(const std::string& name, std::vector<std::vector<uint32_t>> map)
	: Name(name), Tilemap(map), Width(map[0].size()), Height(map.size())
{
	TraverseTilemap(
	[&](uint32_t x, uint32_t y) {
		if(IsLava(x, y))
			m_LavaPoints.push_back({ x, y });
		if(IsGoal(x, y))
			m_Goal = { x, y };
		if(m_Tilemap[y][x] == 5)
			m_PlayerStart = { x, y };
	});
}

void Level::OnUpdate(TimeStep ts) {
	m_TimeStep = ts;

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
		if(IsWall(x, y)) {
			ECS::Entity wall = ECS::EntityBuilder(world)
			.Add<TransformComponent>(Transform{ .Translation = { x, 1.0f, y } })
			.Add<RigidBodyComponent>(RigidBody::Type::Static)
			.Add<MeshComponent>(Asset::Wall)
			.Finalize();
		}
		if(IsPath(x, y)) {
			ECS::Entity floor = ECS::EntityBuilder(world)
			.Add<TransformComponent>(Transform{ .Translation = { x, 0.0f, y } })
			.Add<RigidBodyComponent>(RigidBody::Type::Static)
			.Add<MeshComponent>(Asset::Wall)
			// .Add<MeshComponent>(PickPathMesh(x, y))
			.Finalize();
		}
		if(IsLava(x, y)) {
			ECS::Entity lava = ECS::EntityBuilder(world)
			.Add<TransformComponent>(Transform{ .Translation = { x, 1.0f, y } })
			.Add<RigidBodyComponent>(RigidBody::Type::Static)
			.Add<MeshComponent>(Asset::Wall)
			// .Add<MeshComponent>(PickPathMesh(x, y))
			.Finalize();
		}
		if(IsGoal(x, y)) {
			ECS::Entity stairs = ECS::EntityBuilder(world, "Goal")
			.Add<TransformComponent>(Transform{ .Translation = { x, 1.0f, y } })
			.Add<RigidBodyComponent>(RigidBody::Type::Static)
			.Add<MeshComponent>(Asset::Stairs)
			.Finalize();
		}
	});

	return scene;
}

void Level::TraverseTilemap(
	const std::function<void(uint32_t x, uint32_t y)>& func)
{
	for(uint32_t i = 0; i < Height; i++)
		for(uint32_t j = 0; j < Width; j++)
			func(j, i);
}

bool Level::InBounds(uint32_t col, uint32_t row) {
	return (0 <= col) && (col < Width)
		&& (0 <= row) && (row < Height)
}

bool Level::IsWall(uint32_t col, uint32_t row) {
	return m_Tilemap[row][col] == 0;
}
bool Level::IsPath(uint32_t col, uint32_t row) {
	return m_Tilemap[row][col] == 1
		&& m_Tilemap[row][col] == 5;
}
bool Level::IsLava(uint32_t col, uint32_t row) {
	return m_Tilemap[row][col] == 2;
}
bool Level::IsGoal(uint32_t col, uint32_t row) {
	return m_Tilemap[row][col] == 3;
}
bool Level::IsCheckpoint(uint32_t col, uint32_t row) {
	return m_Tilemap[row][col] == 4;
}

}