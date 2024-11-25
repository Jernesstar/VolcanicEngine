#pragma once

#include <glad/glad.h>

#include <VolcaniCore/Core/Buffer.h>
#include <VolcaniCore/Core/Assert.h>

// #include "Object/UniformBuffer.h"

#include "Renderer/BufferLayout.h"

using namespace VolcaniCore;

namespace VolcaniCore::OpenGL {

class UniformBuffer : Buffer{
public:
	// const std::string Name;
	const BufferLayout Layout;
	const uint32_t Count;
	const uint32_t Size;
	const uint32_t Binding;

public:
	UniformBuffer(const BufferLayout& layout, uint32_t binding,
				  uint32_t count = 1, const void* data = nullptr)
		: Layout(layout), Binding(binding),
			Count(count), Size(count * layout.Stride)
	{
		glCreateBuffers(1, &m_BufferID);
		glNamedBufferData(m_BufferID, Size, data, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, binding, m_BufferID);
	}

	template<typename T>
	UniformBuffer(const BufferLayout& layout, uint32_t binding,
				  const Buffer<T>& buffer)
		: Layout(layout), Binding(binding),
			Count(buffer.GetCount()), Size(buffer.GetSize())
	{
		VOLCANICORE_ASSERT(layout.Stride == sizeof(T));

		glCreateBuffers(1, &m_BufferID);
		glNamedBufferData(m_BufferID, Size, buffer.Get(), GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, binding, m_BufferID);
	}

	~UniformBuffer() {
		glDeleteBuffers(1, &m_BufferID);
	}

	void SetData(const void* data, uint32_t count = 1, uint32_t offset = 0) {
		glNamedBufferSubData(m_BufferID, offset * Layout.Stride,
							 count == 0 ? Size : count * Layout.Stride, data);
	}

private:
	uint32_t m_BufferID;
};

}