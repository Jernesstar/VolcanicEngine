#pragma once

#include "Assert.h"

namespace VolcaniCore {

class CommandLineArgs {
public:
	uint32_t Count;

	CommandLineArgs(uint32_t argc, char** argv)
		: Count(argc - 1), m_Args(argc - 1)
	{
		// Skip the first commandline argument, which is the executable itself
		for(uint32_t i = 1; i < argc; i++)
			m_Args[i - 1] = std::string(argv[i]);
	}

	std::string operator [](uint32_t index) const {
		VOLCANICORE_ASSERT(index < Count);
		return m_Args[index];
	}

private:
	std::vector<std::string> m_Args;
};

}