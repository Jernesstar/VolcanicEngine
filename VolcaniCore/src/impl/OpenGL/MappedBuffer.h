#pragma once

#include <glad/glad.h>

#include <VolcaniCore/Core/Assert.h>
#include <VolcaniCore/Core/Buffer.h>

#include "Graphics/BufferLayout.h"

namespace VolcaniCore::OpenGL {

class MappedBuffer {
public:
	const BufferLayout Layout;
	const uint32_t Count;
	const uint32_t Size;

public:
	MappedBuffer(const BufferLayout& layout, uint32_t count)
		: Layout(layout), Count(count), Size(count * layout.Stride)
	{
		uint32_t flags;
		flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
		glCreateBuffers(1, &m_BufferID);
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
		glBufferStorage(GL_ARRAY_BUFFER, Size, 0, flags);

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

	template<typename T>
	Buffer<T> Get() {
		return Buffer<T>{ m_DataPointer, Count };
	}

	template<typename T>
	void SetData(Buffer<T> buffer, uint32_t offset = 0) {
		Buffer currBuffer{ m_DataPointer, buffer.GetCount() };
		currBuffer.Add(buffer);
	}

	void Lock() {
		glDeleteSync(m_Sync);
		m_Sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
	}

	void Wait() {
		GLenum exit = GL_UNSIGNALED;
		while(exit != GL_ALREADY_SIGNALED && exit != GL_CONDITION_SATISFIED) {
			exit = glClientWaitSync(m_Sync, GL_SYNC_FLUSH_COMMANDS_BIT, 1);
		}
	}

private:
	uint32_t m_BufferID;
	GLsync m_Sync;
	void* m_DataPointer;
};

}