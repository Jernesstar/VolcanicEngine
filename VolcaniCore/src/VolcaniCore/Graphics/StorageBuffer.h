#pragma once

#include "Core/Defines.h"
#include "Core/Template.h"
#include "Core/Buffer.h"

#include "Graphics/BufferLayout.h"

namespace VolcaniCore {

class StorageBuffer : public Derivable<StorageBuffer> {
public:
	static Ref<StorageBuffer> Create(const BufferLayout& layout,
									 uint64_t count = 1, void* data = nullptr);

	template<typename T>
	static Ref<StorageBuffer> Create(const BufferLayout& layout,
									 const Buffer<T>& data)
	{
		return Create(layout, data.GetCount(), data.Get());
	}

public:
	const BufferLayout Layout;
	const uint64_t Count;

public:
	StorageBuffer(const BufferLayout& layout, uint64_t count = 1)
		: Layout(layout), Count(count) { }
	virtual ~StorageBuffer() = default;

	template<typename T>
	void SetData(const Buffer<T>& buffer, uint64_t offset = 0) {
		VOLCANICORE_ASSERT(sizeof(T) == Layout.Stride);
		SetData(buffer.Get(), buffer.GetCount(), offset);
	}

	virtual void SetData(const void* data, uint64_t count = 1,
						 uint64_t offset = 0) = 0;
};

}