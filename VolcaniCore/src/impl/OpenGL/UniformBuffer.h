#pragma once

#include <glad/glad.h>

#include "Renderer/BufferLayout.h"

namespace VolcaniCore::OpenGL {

class UniformBuffer {
public:
	const uint32_t Binding;
	const uint32_t Size;

public:
	UniformBuffer(uint32_t binding, std::size_t size, const void* data = nullptr, bool dynamic = true)
		: Binding(binding), Size(size)
	{
		glCreateBuffers(1, &m_BufferID);
		glNamedBufferData(m_BufferID, size, data, dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
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