#pragma once

#include <vector>

#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "BufferLayout.h"

namespace VolcaniCore {

class VertexArray {
public:
    VertexArray();
    VertexArray(VertexBuffer* vertex_buffer, IndexBuffer* index_buffer = nullptr);
    VertexArray(std::initializer_list<VertexBuffer*> vertex_buffers, IndexBuffer* index_buffer = nullptr);
    ~VertexArray();

    void Bind() const;
    void Unbind() const;

    void AddVertexBuffer(VertexBuffer* vertex_buffer);
    void SetIndexBuffer(IndexBuffer* index_buffer);

    std::vector<VertexBuffer*> GetVertexBuffer() const { return m_VertexBuffers; }
    IndexBuffer* GetIndexBuffer() const { return m_IndexBuffer; }
    bool HasIndexBuffer() const { return m_IndexBuffer != nullptr; }

private:
    uint32_t m_VertexArrayID;
    uint32_t m_BufferIndex = 0;

    std::vector<VertexBuffer*> m_VertexBuffers;
    IndexBuffer* m_IndexBuffer;
};

}