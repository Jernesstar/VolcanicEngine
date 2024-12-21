#pragma once

#include <glad/glad.h>

#include <VolcaniCore/Core/Assert.h>

#include "Graphics/UniformBuffer.h"

using namespace VolcaniCore;

namespace VolcaniCore::OpenGL {

class UniformBuffer : public VolcaniCore::UniformBuffer {
public:
	const uint64_t Size;

public:
	UniformBuffer(const BufferLayout& layout, const void* data = nullptr)
		: VolcaniCore::UniformBuffer(layout), Size(layout.Stride)
	{
		glCreateBuffers(1, &m_BufferID);
		glNamedBufferData(m_BufferID, Size, data, GL_DYNAMIC_DRAW);
	}

	template<typename T>
	UniformBuffer(const BufferLayout& layout, const Buffer<T>& buffer)
		: VolcaniCore::UniformBuffer(layout), Size(layout.Stride)
	{
		VOLCANICORE_ASSERT(layout.Stride == sizeof(T));

		glCreateBuffers(1, &m_BufferID);
		glNamedBufferData(m_BufferID, Size, buffer.Get(), GL_DYNAMIC_DRAW);
	}

	~UniformBuffer() {
		glDeleteBuffers(1, &m_BufferID);
	}

	void SetData(const void* data, uint32_t count = 1, uint32_t offset = 0) {
		glNamedBufferSubData(m_BufferID, offset * Layout.Stride,
							 count == 0 ? Size : count * Layout.Stride, data);
	}

	void Bind(uint32_t binding) {
		glBindBufferBase(GL_UNIFORM_BUFFER, binding, m_BufferID);
	}

private:
	uint32_t m_BufferID;
};

}