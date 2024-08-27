#pragma once

#include <glad/glad.h>

#include <VolcaniCore/Core/Assert.h>
#include <VolcaniCore/Core/Buffer.h>

#include "BufferLayout.h"

namespace VolcaniCore::OpenGL {

class VertexBuffer {
public:
	const BufferLayout Layout;

public:
	VertexBuffer(const BufferLayout& layout,
				 uint32_t count, const void* data = nullptr)
		: Layout(layout)
	{
		glCreateBuffers(1, &m_BufferID);
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
		glBufferData(GL_ARRAY_BUFFER, count * Layout.Stride, data,
					 layout.Dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
	}

	template<typename T>
	VertexBuffer(const BufferLayout& layout, Buffer<T> buffer)
		: Layout(layout)
	{
		VOLCANICORE_ASSERT(layout.Stride == sizeof(T));

		glCreateBuffers(1, &m_BufferID);
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
		glBufferData(GL_ARRAY_BUFFER,
					 buffer.GetSize(), buffer.Get(),
					 layout.Dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
	}

	template<typename T, std::size_t TCount>
	VertexBuffer(const BufferLayout& layout, const T (&vertices)[TCount])
		: Layout(layout)
	{
		glCreateBuffers(1, &m_BufferID);
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
		glBufferData(GL_ARRAY_BUFFER, TCount * Layout.Stride, vertices,
					 layout.Dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
	}

	~VertexBuffer() {
		glDeleteBuffers(1, &m_BufferID);
	}

	void Bind() const {
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
	}
	void Unbind() const {
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void SetData(std::size_t count, const void* data, uint32_t offset = 0) {
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
		glBufferSubData(GL_ARRAY_BUFFER, offset * Layout.Stride,
						count * Layout.Stride, data);
	}

	template<typename T>
	void SetData(Buffer<T> buffer, uint32_t offset = 0) {
		SetData(buffer.GetCount(), buffer.Get(), offset);
	}

	template<typename T, std::size_t TCount>
	void SetData(const T (&vertices)[TCount]) {
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
		glBufferSubData(GL_ARRAY_BUFFER, 0, TCount * Layout.Stride, vertices);
	}

private:
	uint32_t m_BufferID;
};

}