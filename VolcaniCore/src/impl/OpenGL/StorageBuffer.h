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
	StorageBuffer(const BufferLayout& layout, uint64_t count, const void* data)
		: VolcaniCore::StorageBuffer(layout, count), Size(layout.Stride)
	{
		glCreateBuffers(1, &m_BufferID);
		glNamedBufferStorage(m_BufferID, count * Size, data,
			GL_DYNAMIC_STORAGE_BIT);
	}

	~StorageBuffer() {
		glDeleteBuffers(1, &m_BufferID);
	}

	void SetData(const void* data, uint64_t count = 1,
				 uint64_t offset = 0) override
	{
		glNamedBufferSubData(m_BufferID, offset * Size, Size * count, data);
	}

	void Bind(uint32_t binding) {
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_BufferID);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, m_BufferID);
	}

private:
	uint32_t m_BufferID;
};

}