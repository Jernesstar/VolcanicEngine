#pragma once

#include <stdio.h>
#include <string>

namespace VolcaniCore {

class Log {
public:
	template<typename... Args>
	static void Info(const std::string& formatString, Args&&... args)
	{
		printf("[Info]: ");
		printf(formatString.c_str(), std::forward<Args>(args)...);
		printf("\n");
	}

	template<typename... Args>
	static void Warning(const std::string& formatString, Args&&... args)
	{
		printf("[Warning]: ");
		printf(formatString.c_str(), std::forward<Args>(args)...);
		printf("\n");
	}

	template<typename... Args>
	static void Error(const std::string& formatString, Args&&... args)
	{
		printf("[Error]: ");
		printf(formatString.c_str(), std::forward<Args>(args)...);
		printf("\n");
	}
};

}

#define VOLCANICORE_LOG_INFO(message, ...) VolcaniCore::Log::Info(message, ## __VA_ARGS__)
#define VOLCANICORE_LOG_WARNING(message, ...) VolcaniCore::Log::Warning(message, ## __VA_ARGS__)
#define VOLCANICORE_LOG_ERROR(message, ...) VolcaniCore::Log::Error(message, ## __VA_ARGS__)