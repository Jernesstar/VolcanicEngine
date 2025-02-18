#pragma once

#include <string>

#ifdef VOLCANICENGINE_WINDOWS
	#include <windows.h>
#elif VOLCANICENGINE_LINUX
	#include <unistd.h>
#endif

namespace Lava {

std::string FindExecutablePath() {
#ifdef VOLCANICENGINE_WINDOWS
	char buf[2*MAX_PATH];
	size_t len = GetModuleFileName(0, buf, 2*MAX_PATH);
#elif VOLCANICENGINE_LINUX
	char buf[2*PATH_MAX];
	size_t len = readlink("/proc/self/exe", buf, 2*PATH_MAX);
// #elif VOLCANICENGINE_APPLE
	// NS_GetExecutablePath
#endif
	return std::string(buf, len);
}

}