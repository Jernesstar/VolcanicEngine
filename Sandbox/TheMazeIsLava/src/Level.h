#pragma once

#include <glm/vec2.hpp>

#include <VolcaniCore/Core/Defines.h>

#include <VolcaniCore/Core/Time.h>
#include <VolcaniCore/Object/Texture.h>

#include <Magma/Scene/Scene.h>

using namespace VolcaniCore;
using namespace Magma;

namespace TheMazeIsLava {

struct Tile { uint32_t x, y; };
using Tilemap = List<List<uint32_t>>;

class Level {
public:
	const std::string Name;
	const uint32_t Width, Height;

	Tilemap Map;
	Tile Goal;
	Tile PlayerStart;
	List<Tile> LavaPoints;

	bool Paused   = false;
	bool GameOver = false;
	bool Complete = false;

public:
	Level(const std::string& name, const Tilemap& map);
	~Level() = default;

	void OnUpdate(TimeStep ts);
	void OnRender();

	void Load();
	Ref<Scene> GetScene() const { return m_Scene; }

	void TraverseTilemap(const Func<const Tile&, void>& func);

	bool IsWall(const Tile& tile) const;
	bool IsPath(const Tile& tile) const;
	bool IsLava(const Tile& tile) const;
	bool IsGoal(const Tile& tile) const;
	bool IsStart(const Tile& tile) const;
	bool IsCheckpoint(const Tile& tile) const;
	bool IsInbounds(const Tile& tile) const;

private:
	void PropagateLava(TimeStep ts);

private:
	Ref<Scene> m_Scene = nullptr;
};

}