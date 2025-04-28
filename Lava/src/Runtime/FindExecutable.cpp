#include <string>

#include <VolcaniCore/Core/Defines.h>

#ifdef VOLCANICENGINE_WINDOWS
	#include <windows.h>
#elif VOLCANICENGINE_LINUX
	#include <unistd.h>
#endif

namespace Lava {

std::string FindExecutablePath() {
#ifdef VOLCANICENGINE_WINDOWS
#ifndef _MSC_VER
	char buf[2*MAX_PATH];
	size_t len = GetModuleFileName(0, buf, 2*MAX_PATH);
#else
	wchar_t buffer[2*MAX_PATH];
	size_t len = GetModuleFileName(0, buffer, 2*MAX_PATH);
	std::wstring w(buffer, len);
	std::string s(w.begin(), w.end());
	char* buf = (char*)s.c_str();
#endif
#elif VOLCANICENGINE_LINUX
	char buf[2*PATH_MAX];
	size_t len = readlink("/proc/self/exe", buf, 2*PATH_MAX);
// #elif VOLCANICENGINE_APPLE
	// NS_GetExecutablePath
#endif
	return std::string(buf, len);
}

}