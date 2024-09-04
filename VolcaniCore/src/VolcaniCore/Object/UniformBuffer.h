#pragma once

#include <functional>

#include "Core/Defines.h"

namespace VolcaniCore {

template<typename TUniform, typename THandle>
class UniformBuffer {
public:
	std::function<TUniform(Ref<THandle)>> SetData = [](){};

public:
	UniformBuffer() = default;
	UniformBuffer(Ref<THandle> handle)
		: m_Handle(handle) { }

	virtual ~UniformBuffer() = default;

protected:
	Ref<THandle> m_Handle;
};

}