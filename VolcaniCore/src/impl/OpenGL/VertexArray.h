#pragma once

#include <vector>

#include <VolcaniCore/Core/Defines.h>
#include <VolcaniCore/Renderer/BufferLayout.h>

#include "VertexBuffer.h"
#include "IndexBuffer.h"

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

	void SetIndexBuffer(Ref<IndexBuffer> buffer);
	void AddVertexBuffer(Ref<VertexBuffer> buffer);

	bool HasIndexBuffer() const { return bool(m_IndexBuffer); }
	Ref<IndexBuffer> GetIndexBuffer() const { return m_IndexBuffer; }

	Ref<VertexBuffer> GetVertexBuffer(uint32_t index = 0) {
		return m_VertexBuffers[index];
	}

private:
	uint32_t m_VertexArrayID;
	uint32_t m_BufferIndex = 0;

	std::vector<Ref<VertexBuffer>> m_VertexBuffers;
	Ref<IndexBuffer> m_IndexBuffer;
};

}