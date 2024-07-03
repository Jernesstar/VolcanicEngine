#include "VertexArray.h"

#include <iostream>
#include <cstring>

#include <glad/glad.h>

#include "Core/Log.h"

namespace VolcaniCore::OpenGL {

VertexArray::VertexArray() {
	glCreateVertexArrays(1, &m_VertexArrayID);
}

VertexArray::VertexArray(VertexBuffer* vertexBuffer, IndexBuffer* indexBuffer) {
	glCreateVertexArrays(1, &m_VertexArrayID);
	AddVertexBuffer(vertexBuffer);
	SetIndexBuffer(indexBuffer);
}

VertexArray::VertexArray(std::initializer_list<VertexBuffer*> vertexBuffers,
						 IndexBuffer* indexBuffer)
{
	glCreateVertexArrays(1, &m_VertexArrayID);
	SetIndexBuffer(indexBuffer);

	for(auto* buffer : vertexBuffers)
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

void VertexArray::AddVertexBuffer(VertexBuffer* vertexBuffer) {
	if(!vertexBuffer) {
		VOLCANICORE_LOG_WARNING("Vertex buffer is nullptr");
		return;
	}

	glBindVertexArray(m_VertexArrayID);
	vertexBuffer->Bind();

	uint64_t offset = 0;
	const auto& layout = vertexBuffer->Layout;
	uint32_t stride = layout.Stride;

	// if(layout.Elements.size() == 1) // Structure of Arrays
	// 	stride = 0;

	for(auto& element : layout) {
		switch(element.Type) {
			case BufferDataType::Float:
			case BufferDataType::Vec2:
			case BufferDataType::Vec3:
			case BufferDataType::Vec4:
			{
				glEnableVertexAttribArray(m_BufferIndex);
				glVertexAttribPointer(m_BufferIndex++, element.Count, GL_FLOAT,
									  element.Normalized ? GL_FALSE : GL_TRUE,
									  stride, (void*)offset);
				break;
			}

			case BufferDataType::Int:
			{
				glEnableVertexAttribArray(m_BufferIndex);
				glVertexAttribIPointer(m_BufferIndex++, element.Count, GL_INT,
									   stride, (void*)offset);

				break;
			}

			case BufferDataType::Mat2:
			case BufferDataType::Mat3:
			case BufferDataType::Mat4:
			{
				for(uint32_t i = 0; i < element.Count; i++) {
					glEnableVertexAttribArray(m_BufferIndex);
					glVertexAttribPointer(m_BufferIndex, element.Count,
						GL_FLOAT, element.Normalized ? GL_FALSE : GL_TRUE,
						stride,
						(void*)(offset + (sizeof(float) * element.Count * i)));

					glVertexAttribDivisor(m_BufferIndex++, 1);
				}
				break;
			}
		}
		offset += element.Size;
	}

	m_VertexBuffers.push_back(vertexBuffer);
}

void VertexArray::SetIndexBuffer(IndexBuffer* indexBuffer) {
	if(!indexBuffer)
		return;

	glBindVertexArray(m_VertexArrayID);
	indexBuffer->Bind();
	m_IndexBuffer = indexBuffer;
}

}