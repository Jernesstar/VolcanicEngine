#pragma once

#include <cstdint>

namespace VolcaniCore {

class Random {
public:
	static uint64_t RandUInt();
	static uint64_t RandUInt(uint64_t low, uint64_t high);
	static int64_t RandInt();
	static int64_t RandInt(int64_t low, int64_t high);
};

}