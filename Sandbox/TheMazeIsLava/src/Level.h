
#pragma once

#include <cstdint>
#include <vector>
#include <functional>

#include <glm/vec2.hpp>

#include <VolcaniCore/Core/Time.h>
#include <VolcaniCore/Object/Texture.h>

#include <Magma/Scene/Scene.h>

using namespace VolcaniCore;
using namespace Magma;

namespace TheMazeIsLava {

struct Tile { uint32_t x, y };
using Coordinate = glm::vec2;
using TileMap	 = List<List<uint32_t>>;

class Level {
public:
	const std::string Name;
	const uint32_t Width, Height;

	TileMap Map;
	Coordinate Goal;
	Coordinate PlayerStart;
	List<Tile> LavaPoints;

public:
	Level(const std::string& name, const TileMap& map);
	~Level() = default;

	void OnUpdate(TimeStep ts);

	void Load();
	Ref<Scene> GetScene() { return m_Scene; }

	void TraverseTilemap(const std::function<void(const Tile& tile)>& func);

	bool IsWall(const Tile& tile) const;
	bool IsPath(const Tile& tile) const;
	bool IsLava(const Tile& tile) const;
	bool IsGoal(const Tile& tile) const;
	bool IsStart(const Tile& tile) const;
	bool IsCheckpoint(const Tile& tile) const;
	bool IsInbounds(const Tile& tile) const;

private:
	Ref<Scene> m_Scene;

	void PropagateLava(TimeStep ts);
};

}