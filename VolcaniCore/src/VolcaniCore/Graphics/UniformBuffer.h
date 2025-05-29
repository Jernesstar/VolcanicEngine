#pragma once

#include "Core/Defines.h"
#include "Core/Template.h"
#include "Core/Buffer.h"

#include "Graphics/BufferLayout.h"

namespace VolcaniCore {

class UniformBuffer : public Derivable<UniformBuffer> {
public:
	static Ref<UniformBuffer> Create(const BufferLayout& layout,
									 uint64_t count = 1, void* data = nullptr);

public:
	const BufferLayout Layout;
	const uint64_t Count;

public:
	UniformBuffer(const BufferLayout& layout, uint64_t count = 1)
		: Layout(layout), Count(count) { }
	virtual ~UniformBuffer() = default;

	template<typename T>
	void SetData(const Buffer<T>& buffer, uint64_t offset = 0) {
		VOLCANICORE_ASSERT(sizeof(T) == Layout.Stride);
		SetData(buffer.Get(), buffer.GetCount(), offset);
	}

	virtual void SetData(const void* data, uint64_t count = 1,
						 uint64_t offset = 0) = 0;
};

}