#pragma once

#include <cstdint>
#include <vector>

#include <glm/vec2.hpp>

namespace TheMazeIsLava {

using Coordinate = glm::vec2;

class Level {
public:
	Level(std::vector<std::vector<uint32_t>> map);
	void Render();

private:
	Coordinate m_Goal;
	std::vector<Coordinate> m_LavaPoints;
};

}