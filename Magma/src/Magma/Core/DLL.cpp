#include "DLL.h"

#if VOLCANICENGINE_WINDOWS
	#include "windows.h"
#else
#endif

namespace Magma {

DLL::DLL(const std::string& path) {
#if VOLCANICENGINE_WINDOWS
	m_Handle = LoadLibraryA(path.c_str());
#else
	void* hGetProcIDDLL = dlopen(sDllPath.c_str(), RTLD_LAZY);
#endif

}

DLL::~DLL() {
#if VOLCANICENGINE_WINDOWS
	
#else
	dlClose(hGetProcIDDLL);
#endif
}

}