#pragma once

#include <VolcaniCore/Core/Defines.h>

namespace Magma {

template<typename TReturn, class ...Args>
using Function = TReturn(*)(Args...);

class DLL {
public:
	DLL(const std::string& path);
	~DLL();

	template<typename TReturn, typename ...Args>
	Function<TReturn, Args...> GetFunction(const std::string& name) {
		return (Function<TReturn, Args...>)GetFunc(m_Handle, name);
	}

private:
	void* m_Handle;

private:
	static Function<void> GetFunc(void* handle, const std::string& name);
};

}