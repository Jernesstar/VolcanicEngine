#pragma once

#include <glad/glad.h>

namespace VolcaniCore::OpenGL {

class IndexBuffer {
public:
	const uint32_t Count;

	template<std::size_t Count>
	IndexBuffer(const uint32_t (&indices)[Count])
		: Count(Count)
	{
		glCreateBuffers(1, &m_IndexBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, m_IndexBufferID);
		glBufferData(GL_ARRAY_BUFFER, Count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	IndexBuffer(const uint32_t* indices, uint32_t count)
		: Count(count)
	{
		glCreateBuffers(1, &m_IndexBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, m_IndexBufferID);
		glBufferData(GL_ARRAY_BUFFER, Count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	~IndexBuffer() { glDeleteBuffers(1, &m_IndexBufferID); }

	void Bind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBufferID); }
	void Unbind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }

private:
	uint32_t m_IndexBufferID;
};

}