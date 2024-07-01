#pragma once

#include <glad/glad.h>

#include "Object/UniformBuffer.h"

#include "BufferLayout.h"

namespace VolcaniCore::OpenGL {

class UniformBuffer : public VolcaniCore::UniformBuffer {
public:
	const uint32_t Binding;
	const uint32_t Size;

public:
	UniformBuffer() : Size(0), Binding(0) { }
	UniformBuffer(uint32_t binding, std::size_t size, const void* data = nullptr)
		: Binding(binding), Size(size)
	{
		glCreateBuffers(1, &m_BufferID);
		glNamedBufferData(m_BufferID, size, data, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, binding, m_BufferID);
	}

	~UniformBuffer() { glDeleteBuffers(1, &m_BufferID); }

	void SetData(const void* data, uint32_t size = 0, uint32_t offset = 0)
	{
		glNamedBufferSubData(m_BufferID, offset, size == 0 ? Size : size, data);
	}

private:
	uint32_t m_BufferID;
};

}