#pragma once

#include <glad/gl.h>

#include "Core/Buffer.h"

namespace VolcaniCore::OpenGL {

class IndexBuffer {
public:
	const uint32_t Count;
	const uint32_t Size;

public:
	IndexBuffer(uint32_t count, bool dynamic = false,
				const uint32_t* indices = nullptr)
		: Count(count), Size(count * sizeof(uint32_t))
	{
		glCreateBuffers(1, &m_BufferID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, Size, indices,
					 dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	IndexBuffer(const Buffer<uint32_t>& buffer, bool dynamic = false)
		: Count(buffer.GetCount()), Size(buffer.GetSize())
	{
		glCreateBuffers(1, &m_BufferID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, Size, buffer.Get(),
					 dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	template<std::size_t TCount>
	IndexBuffer(const uint32_t (&indices)[TCount])
		: Count(TCount), Size(TCount * sizeof(uint32_t))
	{
		glCreateBuffers(1, &m_BufferID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, Size, indices, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	~IndexBuffer() {
		glDeleteBuffers(1, &m_BufferID);
	}

	void Bind() const {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferID);
	}
	void Unbind() const {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void SetData(const void* data, uint32_t count = 0, uint32_t offset = 0) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferID);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset * sizeof(uint32_t),
						(count == 0) ? Size : count * sizeof(uint32_t), data);
		
	}

	void SetData(const Buffer<uint32_t>& buffer, uint32_t offset = 0) {
		SetData(buffer.Get(), buffer.GetCount(), offset);
	}

	template<std::size_t TCount>
	void SetData(const uint32_t (&indices)[TCount]) {
		SetData(indices, TCount);
	}

private:
	uint32_t m_BufferID;
};

}