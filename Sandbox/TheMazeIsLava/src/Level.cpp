#include "Level.h"

#include <Magma/ECS/EntityBuilder.h>

#include "Asset.h"

using namespace Magma::ECS;
using namespace Magma::Physics;

namespace TheMazeIsLava {

Level::Level(const std::string& name, std::vector<std::vector<uint32_t>> map)
	: Name(name), Width(map[0].size()), Height(map.size()), Map(map)
{
	TraverseTilemap(
	[&](const Tile& tile)
	{
		auto [x, y] = tile;

		if(IsLava(tile))  LavaPoints.push_back({ x, y });
		if(IsGoal(tile))  Goal = { x, y };
		if(IsStart(tile)) PlayerStart = { x, y };
	});
}

void Level::OnUpdate(TimeStep ts) {
	PropagateLava(ts);

	m_Scene->OnUpdate(ts);
}

void Level::PropagateLava(TimeStep ts) {
	// uint32_t lavaSpeed = LavaSpeed(m_TimeSinceLevelStart);

	// Propagate the lava in tile space, flow animation interpolates smoothly
}

void Level::Load() {
	if(m_Scene)
		return;

	m_Scene = CreateRef<Scene>();
	auto& world = scene->GetEntityWorld();

	TraverseTilemap(
		[&](const Tile& tile)
		{
			auto [x, y] = tile;

			if(IsWall(tile)) {
				ECS::Entity wall = ECS::EntityBuilder(world)
				.Add<TransformComponent>(
					Transform{ .Translation = { x, 1.0f, y } }
				)
				.Add<MeshComponent>(Asset::Wall)
				// .Add<RigidBodyComponent>(RigidBody::Type::Static)
				.Finalize();
			}
			if(IsPath(tile)) {
				ECS::Entity path = ECS::EntityBuilder(world)
				.Add<TransformComponent>(
					Transform{ .Translation = { x, 0.0f, y } }
				)
				.Add<MeshComponent>(Asset::Wall)
				.Add<RigidBodyComponent>(RigidBody::Type::Static)
				// .Add<MeshComponent>(PickPathMesh(tile))
				.Finalize();
			}
			if(IsLava(tile)) {
				ECS::Entity lava = ECS::EntityBuilder(world)
				.Add<TransformComponent>(
					Transform{ .Translation = { x, 1.0f, y } }
				)
				.Add<MeshComponent>(Asset::Lava)
				// .Add<RigidBodyComponent>(RigidBody::Type::Static)
				.Finalize();
			}
			if(IsGoal(tile)) {
				ECS::Entity stairs = ECS::EntityBuilder(world, "Goal")
				.Add<TransformComponent>(
					Transform{
						.Translation = { x, 1.0f, y },
						.Scale = glm::vec3(0.5)
					})
				.Add<MeshComponent>(Asset::Stairs)
				// .Add<RigidBodyComponent>(RigidBody::Type::Static)
				.Finalize();
			}
		});
}

void Level::TraverseTilemap(const std::function<void(const Tile& tile)>& func) {
	for(uint32_t i = 0; i < Height; i++)
		for(uint32_t j = 0; j < Width; j++)
			func(Tile(j, i));
}

// 0 -> Wall
// 1 -> Path
// 2 -> Lava
// 3 -> Goal (End)
// 4 -> Start (Begin)
// 5 -> Checkpoint

bool Level::IsWall(const Tile& tile) const {
	auto [col, row] = tile;
	return Tilemap[row][col] == 0;
}
bool Level::IsPath(const Tile& tile) const {
	auto [col, row] = tile;
	return Tilemap[row][col] == 1;
}
bool Level::IsLava(const Tile& tile) const {
	auto [col, row] = tile;
	return Tilemap[row][col] == 2;
}
bool Level::IsGoal(const Tile& tile) const {
	auto [col, row] = tile;
	return Tilemap[row][col] == 3;
}
bool Level::IsStart(const Tile& tile) const {
	auto [col, row] = tile;
	return Tilemap[row][col] == 4;
}
bool Level::IsCheckpoint(const Tile& tile) const {
	auto [col, row] = tile;
	return Tilemap[row][col] == 5;
}
bool Level::IsInbounds(const Tile& tile) const {
	auto [col, row] = tile;
	return (col < Width) && (row < Height);
}

}