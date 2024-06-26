#pragma once

#include "Assert.h"

namespace VolcaniCore {

class CommandLineArgs {
public:
	int Count;

	CommandLineArgs(int argc, char** argv)
		: Count(argc - 1), m_Args(argc - 1)
	{
		// Skip the first commandline argument, which is the executable itself
		for(int i = 1; i < argc; i++)
			m_Args[i - 1] = std::string(argv[i]);
	}

	std::string operator [](int index) const {
		VOLCANICORE_ASSERT(index < Count);
		return m_Args[index];
	}

private:
	std::vector<std::string> m_Args;
};

}