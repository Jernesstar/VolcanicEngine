
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

using Coordinate = glm::vec2;

class Level {
public:
	const std::string Name;
	const std::vector<std::vector<uint32_t>> Tilemap;
	const uint32_t Width, Height;

public:
	Level(const std::string& name, std::vector<std::vector<uint32_t>> map);
	~Level() = default;

	void OnUpdate(TimeStep ts);

	Ref<Scene> Load();

	Coordinate GetPlayerStart() const { return m_PlayerStart; }
	Coordinate GetGoal() const { return m_Goal; }

	void TraverseTilemap(
			const std::function<void(uint32_t x, uint32_t y)>& func);

	bool IsInbounds(uint32_t col, uint32_t row) const;

	bool IsWall(uint32_t col, uint32_t row) const;
	bool IsPath(uint32_t col, uint32_t row) const;
	bool IsLava(uint32_t col, uint32_t row) const;
	bool IsGoal(uint32_t col, uint32_t row) const;
	bool IsCheckpoint(uint32_t col, uint32_t row) const;

private:
	Coordinate m_Goal;
	Coordinate m_PlayerStart;
	std::vector<Coordinate> m_LavaPoints;

private:
	void PropagateLava(TimeStep ts);
};

}