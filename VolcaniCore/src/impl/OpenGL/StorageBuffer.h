#pragma once

#include <glad/gl.h>

#include <VolcaniCore/Core/Assert.h>

#include "Graphics/StorageBuffer.h"

using namespace VolcaniCore;

namespace VolcaniCore::OpenGL {

class StorageBuffer : public VolcaniCore::StorageBuffer {
public:
	const uint64_t Size;

public:
	StorageBuffer(const BufferLayout& layout, uint64_t count = 1,
				  const void* data = nullptr)
		: VolcaniCore::StorageBuffer(layout, count), Size(layout.Stride)
	{
		glCreateBuffers(1, &m_BufferID);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_BufferID);
	}

	template<typename T>
	StorageBuffer(const BufferLayout& layout, const Buffer<T>& buffer)
		: VolcaniCore::StorageBuffer(layout), Size(layout.Stride)
	{
		VOLCANICORE_ASSERT(layout.Stride == sizeof(T));

		glCreateBuffers(1, &m_BufferID);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_BufferID);
		glBufferData(GL_SHADER_STORAGE_BUFFER, Size, buffer.Get(), GL_DYNAMIC_COPY);
	}

	~StorageBuffer() {
		glDeleteBuffers(1, &m_BufferID);
	}

	void SetData(const void* data, uint64_t count = 1,
				 uint64_t offset = 0) override
	{
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_BufferID);
		GLvoid* p = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
		memcpy(p + offset * Size, data, count * Size);
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	}

	void Bind(uint32_t binding) {
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, m_BufferID);
	}

private:
	uint32_t m_BufferID;
};

}