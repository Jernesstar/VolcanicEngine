#include "Random.h"

namespace VolcaniCore {

static std::random_device s_Device;
static std::mt19937 s_RNG{ s_Device() };
static std::uniform_int_distribution<uint64_t> s_Dist{ };

uint32_t Random::RandInt() {
	return s_Dist(s_RNG);
}

uint32_t Random::RandInt(uint32_t low, uint32_t high) {
	std::uniform_int_distribution<uint64_t> dist{ low, high };
	return dist(s_RNG);
}

}