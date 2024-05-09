#pragma once

#include <cstdint>
#include <vector>

struct Coordinate { uint32_t x, y; }

class Level {
public:
	static void LoadLevel(std::vector<std::vector<uint32_t>> map);

private:
	static Coordinate m_Goal;
	static std::vector<Coordinate> m_LavaPoints;
};

