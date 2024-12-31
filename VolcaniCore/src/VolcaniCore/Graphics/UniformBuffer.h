#pragma once

#include "Core/Defines.h"
#include "Core/Buffer.h"

#include "Graphics/BufferLayout.h"

namespace VolcaniCore {

class UniformBuffer {
public:
	const BufferLayout Layout;

	static Ref<UniformBuffer> Create(const BufferLayout& layout);

public:
	UniformBuffer(const BufferLayout& layout)
		: Layout(layout) { }
	virtual ~UniformBuffer() = default;

	template<typename T>
	void SetData(const Buffer<T>& buffer, uint32_t offset = 0) {
		SetData(buffer.Get(), buffer.GetSize(), offset);
	}

	virtual void SetData(const void* data, uint32_t count, uint32_t offset) = 0;

	template<typename TDerived>
	requires std::derived_from<TDerived, UniformBuffer>
	TDerived* As() const { return (TDerived*)(this); }
};

}