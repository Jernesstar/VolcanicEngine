#include "VertexArray.h"

#include <iostream>
#include <cstring>

#include <glad/glad.h>

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

	glBindVertexArray(m_VertexArrayID);
	indexBuffer->Bind();

	m_IndexBuffer = indexBuffer;
}

void VertexArray::AddVertexBuffer(Ref<VertexBuffer> vertexBuffer) {
	VOLCANICORE_ASSERT(vertexBuffer, "Vertex buffer was null");

	glBindVertexArray(m_VertexArrayID);
	vertexBuffer->Bind();

	auto& layout = vertexBuffer->Layout;
	uint32_t stride = layout.Stride;
	uint64_t offset = 0;

	uint32_t count;
	bool normalized;

	for(auto& element : layout) {
		count  = element.Count;
		normalized = element.Normalized ? GL_FALSE : GL_TRUE;

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
				glVertexAttribPointer(
					m_BufferIndex++, count, GL_FLOAT, normalized, stride,
					(void*)offset
				);
				break;
			}
			case BufferDataType::Mat2:
			case BufferDataType::Mat3:
			case BufferDataType::Mat4:
			{
				uint64_t offsetVal = offset;
				uint32_t perInstance = 0; // How many instances before switching vec?
				uint32_t vecSize = sizeof(float) * count;
				if(layout.StructureOfArrays) {
					offsetVal = 0;
					perInstance = 1;
				}
				for(uint32_t i = 0; i < count; i++) {
					glEnableVertexAttribArray(m_BufferIndex);
					glVertexAttribPointer(
						m_BufferIndex, count, GL_FLOAT, normalized, stride,
						(void*)(offsetVal + (vecSize * i))
					);

					glVertexAttribDivisor(m_BufferIndex++, perInstance);
				}
				break;
			}
		}

		offset += element.Size;
	}

	m_VertexBuffers.push_back(vertexBuffer);
}

}