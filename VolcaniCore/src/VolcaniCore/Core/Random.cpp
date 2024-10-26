#include "Random.h"

#include <random>

namespace VolcaniCore {

static std::random_device s_Device;
static std::mt19937 s_RNG{ s_Device() };

uint64_t Random::RandUInt() {
	std::uniform_int_distribution<uint64_t> dist{ };
	return dist(s_RNG);
}

uint64_t Random::RandUInt(uint64_t low, uint64_t high) {
	std::uniform_int_distribution<uint64_t> dist{ low, high };
	return dist(s_RNG);
}

int64_t Random::RandInt() {
	std::uniform_int_distribution<int64_t> dist{ };
	return dist(s_RNG);
}

int64_t Random::RandInt(int64_t low, int64_t high) {
	std::uniform_int_distribution<int64_t> dist{ low, high };
	return dist(s_RNG);
}

}