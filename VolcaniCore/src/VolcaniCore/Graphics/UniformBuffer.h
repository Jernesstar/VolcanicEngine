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
		VOLCANICORE_ASSERT(sizeof(T) == Layout.Stride);
		SetData(buffer.Get(), buffer.GetCount(), offset);
	}

	virtual void SetData(const void* data, uint32_t count = 1, uint32_t offset = 0) = 0;

	template<typename TDerived>
	requires std::derived_from<TDerived, UniformBuffer>
	TDerived* As() const { return (TDerived*)(this); }
};

}