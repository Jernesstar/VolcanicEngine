#pragma once

#include <glad/glad.h>

#include "Core/Assert.h"
#include "Core/Buffer.h"

#include "BufferLayout.h"

namespace VolcaniCore::OpenGL {

class VertexBuffer {
public:
	const BufferLayout Layout;
	const uint32_t Count;
	const uint32_t Size;

public:
	VertexBuffer(const BufferLayout& layout,
				 uint32_t count, const void* data = nullptr)
		: Layout(layout), Count(count), Size(count * layout.Stride)
	{
		glCreateBuffers(1, &m_BufferID);
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
		glBufferData(GL_ARRAY_BUFFER, Size, data,
					 layout.Dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
	}

	template<typename T>
	VertexBuffer(const BufferLayout& layout, Buffer<T> buffer)
		: Layout(layout), Count(buffer.GetCount()), Size(buffer.GetSize())
	{
		VOLCANICORE_ASSERT(layout.Stride == sizeof(T));

		glCreateBuffers(1, &m_BufferID);
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
		glBufferData(GL_ARRAY_BUFFER, buffer.GetSize(), buffer.Get(),
					 layout.Dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
	}

	template<typename T, std::size_t TCount>
	VertexBuffer(const BufferLayout& layout, const T (&vertices)[TCount])
		: Layout(layout), Count(TCount), Size(TCount * layout.Stride)
	{
		glCreateBuffers(1, &m_BufferID);
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
		glBufferData(GL_ARRAY_BUFFER, Size, vertices,
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

	void SetData(const void* data, uint32_t count = 0, uint32_t offset = 0) {
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
		glBufferSubData(GL_ARRAY_BUFFER, offset * Layout.Stride,
						(count == 0) ? Size : count * Layout.Stride, data);
	}

	template<typename T>
	void SetData(Buffer<T> buffer, uint32_t offset = 0) {
		SetData(buffer.Get(), buffer.GetCount(), offset);
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