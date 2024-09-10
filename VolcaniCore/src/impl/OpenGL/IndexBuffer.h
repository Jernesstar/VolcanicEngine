#pragma once

#include <glad/glad.h>

namespace VolcaniCore::OpenGL {

class IndexBuffer {
public:
	const uint32_t Count;
	const uint32_t Size;

	template<std::size_t TCount>
	IndexBuffer(const uint32_t (&indices)[TCount])
		: Count(Count), Size(TCount * sizeof(uint32_t))
	{
		glCreateBuffers(1, &m_BufferID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, Count * sizeof(uint32_t), indices,
					 GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	IndexBuffer(uint32_t count, const uint32_t* indices = nullptr,
				bool dynamic = false)
		: Count(count), Size(count * sizeof(uint32_t))
	{
		glCreateBuffers(1, &m_BufferID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, Count * sizeof(uint32_t), indices,
					 dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
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

	void SetData(Buffer<uint32_t> buffer, uint32_t offset = 0) {
		SetData(buffer.Get(), buffer.GetCount(), offset);
	}

private:
	uint32_t m_BufferID;
};

}