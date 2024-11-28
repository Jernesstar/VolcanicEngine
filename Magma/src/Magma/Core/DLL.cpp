#include "DLL.h"

#if VOLCANICENGINE_WINDOWS
#include <windows.h>
#elif VOLCANICENGINE_LINUX
#include <dlfcn.h>
#endif

#include <VolcaniCore/Core/Assert.h>

namespace Magma {

DLL::DLL(const std::string& path) {
#if VOLCANICENGINE_WINDOWS
	m_Handle = LoadLibraryA(path.c_str());
#elif VOLCANICENGINE_LINUX
	m_Handle = dlopen(path.c_str(), RTLD_LAZY);
#endif

	VOLCANICORE_ASSERT(m_Handle);
}

DLL::~DLL() {
#if VOLCANICENGINE_WINDOWS
	FreeLibrary((HMODULE)m_Handle);
#elif VOLCANICENGINE_LINUX
	dlClose(m_Handle);
#endif
}

Function<void> DLL::GetFunc(void* handle, const std::string& name) {
#if VOLCANICENGINE_WINDOWS
	VOLCANICORE_ASSERT(GetProcAddress((HMODULE)handle, name.c_str()));
	DWORD lastError = GetLastError();
	static char buf[1024];
	FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, lastError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buf, sizeof(buf), NULL);
	VOLCANICORE_LOG_ERROR(buf);

	return (Function<void>)GetProcAddress((HMODULE)handle, name.c_str());
#elif VOLCANICENGINE_LINUX
	return (Function<void>)dlsym(handle, name);
#endif
}

}