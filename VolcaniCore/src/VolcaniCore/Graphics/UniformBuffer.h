#pragma once

#include <functional>

#include "Core/Defines.h"
#include "Core/Buffer.h"
#include "Core/BufferLayout.h"

namespace VolcaniCore {

class UniformBuffer {
public:
	const uint32_t BindingPoint;
	const BufferLayout Layout;

public:
	UniformBuffer(uint32_t binding)
		: BindingPoint(binding) { }
	virtual ~UniformBuffer() = default;

	template<typename T>
	void SetData(const Buffer<T>& buffer, uint32_t offset = 0) {
		SetData(buffer.Get(), buffer.GetSize(), offset);
	}

	virtual void SetData(const void* data, uint32_t count, uint32_t offset) = 0;
};

}