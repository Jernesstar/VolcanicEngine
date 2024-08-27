#pragma once

#include <glad/glad.h>

#include <VolcaniCore/Core/Assert.h>
#include <VolcaniCore/Core/Buffer.h>

#include "BufferLayout.h"

namespace VolcaniCore::OpenGL {

class MappedBuffer {
public:
	const BufferLayout Layout;
	const uint32_t Count;
	const uint32_t Size;

public:
	MappedBuffer(const BufferLayout& layout,
				 uint32_t count, const void* data = nullptr)
		: Layout(layout), Count(count), Size(count * layout.Stride)
	{
		uint32_t flags;
		flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
		glCreateBuffers(1, &m_BufferID);
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
		glBufferStorage(GL_ARRAY_BUFFER, Size, 0, flags);

		m_DataPointer = glMapBufferRange(GL_ARRAY_BUFFER, 0, Size, flags);
	}

	template<typename T>
	MappedBuffer(const BufferLayout& layout, Buffer<T> buffer)
		: Layout(layout), Count(buffer.GetCount()), Size(buffer.GetSize())
	{
		VOLCANICORE_ASSERT(layout.Stride == sizeof(T));

		uint32_t flags;
		flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
		glCreateBuffers(1, &m_BufferID);
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
		glBufferStorage(GL_ARRAY_BUFFER, Size, buffer.Get(), flags);

		m_DataPointer = glMapBufferRange(GL_ARRAY_BUFFER, 0, Size, flags);
	}

	~MappedBuffer() {
		glDeleteBuffers(1, &m_BufferID);
	}

	void Bind() const {
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
	}
	void Unbind() const {
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void Wait() {
		GLenum waitReturn = GL_UNSIGNALED;
		while(waitReturn != GL_ALREADY_SIGNALED
			&& waitReturn != GL_CONDITION_SATISFIED)
		{
			waitReturn = glClientWaitSync(m_Sync, GL_SYNC_FLUSH_COMMANDS_BIT, 1);
		}
	}

	void Lock() {
		glDeleteSync(m_Sync);
		m_Sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
	}

	void SetData(const void* data, uint32_t count, uint32_t offset = 0) {
		// m_DataPointer[offset * Layout.Stride] = memcp();
		// Don't know what to do here
	}

	template<typename T>
	void SetData(Buffer<T> buffer, uint32_t offset = 0) {
		SetData(buffer.Get(), buffer.GetCount(), offset);
	}

private:
	uint32_t m_BufferID;
	void* m_DataPointer;
	GLsync m_Sync;
};

}