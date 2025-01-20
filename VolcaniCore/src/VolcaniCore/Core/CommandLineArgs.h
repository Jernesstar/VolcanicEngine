#pragma once

#include "Assert.h"

#include "List.h"

namespace VolcaniCore {

struct ArgList {
	bool Valid = false;
	List<std::string> Args;

	operator std::string() const { return Args[0]; }
	operator bool() const { return Valid && Args; }

	List<std::string>::iterator begin() { return Args.begin(); }
	List<std::string>::iterator end() { return Args.end(); }
	List<std::string>::const_iterator begin() const { return Args.cbegin(); }
	List<std::string>::const_iterator end()	const { return Args.cend(); }
};

class CommandLineArgs {
public:
	uint32_t Count;

	CommandLineArgs(uint32_t argc, char** argv)
		: Count(argc - 1), m_Args(argc - 1)
	{
		int32_t lastOption = -1;
		// Skip the first command line argument, which is the executable itself
		for(uint32_t i = 0; i < argc - 1; i++) {
			m_Args.Add(std::string(argv[i + 1]));
			if(m_Args[i][0] == '-') {
				lastOption = i;
				m_ArgMap[m_Args[lastOption]] = ArgList(true);
			}
			else if(lastOption != -1)
				m_ArgMap[m_Args[lastOption]].Args.Add(m_Args[i]);
		}

		VOLCANICORE_LOG_INFO("Here");
	}

	std::string operator [](uint32_t index) const {
		VOLCANICORE_ASSERT(index < Count);
		return m_Args[index];
	}

	ArgList operator [](const std::string& option) const {
		if(!Has(option))
			return ArgList(false);
		return m_ArgMap.at(option);
	}

	bool Has(const std::string& option) const {
		return m_ArgMap.count(option);
	}

private:
	List<std::string> m_Args;
	std::unordered_map<std::string, ArgList> m_ArgMap;
};

}