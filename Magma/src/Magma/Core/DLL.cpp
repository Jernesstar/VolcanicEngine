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
	m_Handle = dlopen(path.c_str(), RTLD_NOW);
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

FuncPtr<void> DLL::GetFuncPtr(void* handle, const std::string& name) {
#if VOLCANICENGINE_WINDOWS
	FARPROC funcPtr = GetProcAddress((HMODULE)handle, name.c_str());
	DWORD lastError = GetLastError();
	static char buf[1024];
	FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				   NULL, lastError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				   buf, sizeof(buf), NULL);
	VOLCANICORE_ASSERT_ARGS(funcPtr, "GetFuncPtr: %s", buf);

	return (FuncPtr<void>)funcPtr;
#elif VOLCANICENGINE_LINUX
	return (FuncPtr<void>)dlsym(handle, name.c_str());
#endif
}

}