#pragma once

#include <vector>

#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "BufferLayout.h"

namespace VolcaniCore::OpenGL {

class VertexArray {
public:
	VertexArray();
	VertexArray(Ref<VertexBuffer> vertexBuffer,
				Ref<IndexBuffer> indexBuffer = nullptr);
	VertexArray(std::initializer_list<Ref<VertexBuffer>> vertexBuffers,
				Ref<IndexBuffer> indexBuffer = nullptr);
	~VertexArray();

	void Bind() const;
	void Unbind() const;

	void AddVertexBuffer(Ref<VertexBuffer> vertexBuffer);
	void SetIndexBuffer(Ref<IndexBuffer> index_buffer);

	Ref<IndexBuffer> GetIndexBuffer() const { return m_IndexBuffer; }
	bool HasIndexBuffer() const { return bool(m_IndexBuffer); }

private:
	uint32_t m_VertexArrayID;
	uint32_t m_BufferIndex = 0;

	std::vector<Ref<VertexBuffer>> m_VertexBuffers;
	Ref<IndexBuffer> m_IndexBuffer;
};

}