#pragma once

#include <cstdint>
#include <vector>
#include <functional>

#include <glm/vec2.hpp>

#include <Core/Time.h>
#include <Renderer/Texture.h>

using namespace VolcaniCore;

namespace TheMazeIsLava {

using Coordinate = glm::vec2;

class Level {
public:
	// Return the speed of the lava as a function of the time elapsed since the level started
	const std::function<float(float timeSinceLevelStart)> LavaSpeed;

public:
	static void Init();
	static Level Create(const std::string& levelPath);

	Level(std::vector<std::vector<uint32_t>> map,
		const std::function<float(float timeSinceLevelStart)>& lavaSpeed);
	void Render(TimeStep ts);

private:
	void PropagateLava();

	TimeStep m_TimeStep;
	float m_TimeSinceLevelStart;
	const uint32_t m_Width, m_Height;

	Coordinate m_Goal;
	std::vector<Coordinate> m_LavaPoints;
	std::vector<std::vector<uint32_t>> m_TileMap;

	inline static Ref<Texture> s_Lava;
	inline static Ref<Texture> s_Stone;
	inline static Ref<Texture> s_Door;
};

}