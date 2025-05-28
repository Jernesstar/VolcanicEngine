#pragma once

#include <glad/gl.h>

#include <VolcaniCore/Core/Assert.h>

#include "Graphics/UniformBuffer.h"

using namespace VolcaniCore;

namespace VolcaniCore::OpenGL {

class UniformBuffer : public VolcaniCore::UniformBuffer {
public:
	const uint64_t Size;

	public:
	UniformBuffer(const BufferLayout& layout, uint64_t count = 1,
		const void* data = nullptr)
		: VolcaniCore::UniformBuffer(layout, count), Size(layout.Stride)
	{
		glCreateBuffers(1, &m_BufferID);
		glNamedBufferData(m_BufferID, Size * count, data, GL_DYNAMIC_DRAW);
	}

	template<typename T>
	UniformBuffer(const BufferLayout& layout, const Buffer<T>& buffer)
		: VolcaniCore::UniformBuffer(layout, buffer.GetCount()),
			Size(layout.Stride)
	{
		VOLCANICORE_ASSERT(layout.Stride == sizeof(T));

		glCreateBuffers(1, &m_BufferID);
		glNamedBufferData(m_BufferID, Size * buffer.GetCount(), buffer.Get(),
			GL_DYNAMIC_DRAW);
	}

	~UniformBuffer() {
		glDeleteBuffers(1, &m_BufferID);
	}

	void SetData(const void* data, uint64_t count = 1,
				 uint64_t offset = 0) override
	{
		glNamedBufferSubData(m_BufferID, offset * Size, count * Size, data);
	}

	void Bind(uint32_t binding) {
		glBindBufferBase(GL_UNIFORM_BUFFER, binding, m_BufferID);
	}

private:
	uint32_t m_BufferID;
};

}