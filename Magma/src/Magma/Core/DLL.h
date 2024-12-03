#pragma once

#include <VolcaniCore/Core/Defines.h>

namespace Magma {

template<typename TReturn, class ...Args>
using FuncPtr = TReturn(*)(Args...);

class DLL {
public:
	DLL(const std::string& path);
	~DLL();

	template<typename TReturn, typename ...Args>
	FuncPtr<TReturn, Args...> GetFunction(const std::string& name) {
		return (FuncPtr<TReturn, Args...>)GetFuncPtr(m_Handle, name);
	}

private:
	void* m_Handle;

private:
	static FuncPtr<void> GetFuncPtr(void* handle, const std::string& name);
};

}