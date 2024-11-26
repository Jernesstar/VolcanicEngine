#pragma once

#include <VolcaniCore/Core/Defines.h>

namespace Magma {

template<typename TReturn, class ...Args>
using Function = TReturn(*)(Args...);

Function<void> Func;

class DLL {
public:
	DLL(const std::string& path);
	~DLL();

	template<typename TReturn, class ...Args>
	Function<TReturn, Args...> GetFunction(const std::string& name);

private:
	void* m_Handle;
};

}