#pragma once

#include <cstdint>
#include <vector>
#include <functional>

#include <glm/vec2.hpp>

#include <VolcaniCore/Core/Time.h>

#include <VolcaniCore/Object/Texture.h>

#include <Magma/Editor/Scene.h>

using namespace VolcaniCore;
using namespace Magma;

namespace TheMazeIsLava {

using Coordinate = glm::vec2;

class Level {
public:
	const std::string Name;
	// Return the speed of the lava as a function
	// of the time elapsed since the level started
	// const std::function<float(float t)> LavaSpeed;

	// TODO: Checkpoints
public:
	Level(const std::string& name, std::vector<std::vector<uint32_t>> map);
	~Level();

	std::vector<std::vector<uint32_t>> GetTilemap() const { return m_Tilemap; }

	Ref<Scene> Load();

	void Run(TimeStep ts);

private:
	void PropagateLava();
	void TraverseTilemap(
			const std::function<void(uint32_t x, uint32_t y)>& func);

	bool IsCheckpoint(uint32_t col, uint32_t row);
	bool IsPath(uint32_t col, uint32_t row);
	bool IsLava(uint32_t col, uint32_t row);
	bool IsGoal(uint32_t col, uint32_t row);
	bool IsWall(uint32_t col, uint32_t row);

	TimeStep m_TimeStep;
	float m_TimeSinceLevelStart;
	const uint32_t m_Width, m_Height;

	Coordinate m_Goal;
	std::vector<Coordinate> m_LavaPoints;
	std::vector<std::vector<uint32_t>> m_Tilemap;
};

}