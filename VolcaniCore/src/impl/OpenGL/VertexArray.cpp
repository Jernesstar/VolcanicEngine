#include "VertexArray.h"

#include <iostream>
#include <cstring>

#include <glad/gl.h>

#include "Core/Assert.h"

using namespace VolcaniCore;

namespace VolcaniCore::OpenGL {

VertexArray::VertexArray() {
	glCreateVertexArrays(1, &m_VertexArrayID);
}

VertexArray::VertexArray(Ref<VertexBuffer> vertexBuffer,
						 Ref<IndexBuffer> indexBuffer)
{
	glCreateVertexArrays(1, &m_VertexArrayID);
	AddVertexBuffer(vertexBuffer);
	SetIndexBuffer(indexBuffer);
}

VertexArray::VertexArray(std::initializer_list<Ref<VertexBuffer>> vertexBuffers,
						 Ref<IndexBuffer> indexBuffer)
{
	glCreateVertexArrays(1, &m_VertexArrayID);
	SetIndexBuffer(indexBuffer);

	for(auto buffer : vertexBuffers)
		AddVertexBuffer(buffer);
}

VertexArray::~VertexArray() {
	glDeleteVertexArrays(1, &m_VertexArrayID);
}

void VertexArray::Bind() const {
	glBindVertexArray(m_VertexArrayID);
}

void VertexArray::Unbind() const {
	glBindVertexArray(0);
}

void VertexArray::SetIndexBuffer(Ref<IndexBuffer> indexBuffer) {
	if(!indexBuffer)
		return;

	m_IndexBuffer = indexBuffer;
	Bind();
	indexBuffer->Bind();
	Unbind();
}

void VertexArray::AddVertexBuffer(Ref<VertexBuffer> vertexBuffer) {
	VOLCANICORE_ASSERT(vertexBuffer, "Vertex buffer was null");
	m_VertexBuffers.push_back(vertexBuffer);

	Bind();
	vertexBuffer->Bind();

	auto& layout = vertexBuffer->Layout;
	uint64_t stride = layout.Stride;
	uint64_t offset = 0;

	for(auto& element : layout) {
		uint64_t count = element.Count;
		bool normalized = element.Normalized ? GL_TRUE : GL_FALSE;

		switch(element.Type) {
			case BufferDataType::Int:
			{
				glEnableVertexAttribArray(m_BufferIndex);
				glVertexAttribIPointer(
					m_BufferIndex++, count, GL_INT, stride, (void*)offset);
				break;
			}
			case BufferDataType::Float:
			case BufferDataType::Vec2:
			case BufferDataType::Vec3:
			case BufferDataType::Vec4:
			{
				glEnableVertexAttribArray(m_BufferIndex);
				glVertexAttribPointer(m_BufferIndex, count, GL_FLOAT,
									  normalized, stride, (void*)offset);
				if(layout.StructureOfArrays) // Instanced
					glVertexAttribDivisor(m_BufferIndex, 1);

				m_BufferIndex++;
				break;
			}
			case BufferDataType::Mat2:
			case BufferDataType::Mat3:
			case BufferDataType::Mat4:
			{
				bool instanced = layout.StructureOfArrays;
				uint64_t vecSize = sizeof(float) * count;

				for(uint64_t i = 0; i < count; i++) {
					glEnableVertexAttribArray(m_BufferIndex);
					glVertexAttribPointer(
						m_BufferIndex, count, GL_FLOAT, normalized, stride,
						(void*)(offset + (vecSize * i)));

					glVertexAttribDivisor(m_BufferIndex++, (uint32_t)instanced);
				}
				break;
			}
		}

		offset += element.Size;
	}

	Unbind();
}

}