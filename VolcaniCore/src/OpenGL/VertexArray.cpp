#include "VertexArray.h"

#include <iostream>
#include <cstring>

#include <glad/glad.h>

#include "Core/Log.h"

namespace VolcaniCore::OpenGL {

VertexArray::VertexArray()
{
	glCreateVertexArrays(1, &m_VertexArrayID);
}

VertexArray::VertexArray(VertexBuffer* vertex_buffer, IndexBuffer* index_buffer)
{
	glCreateVertexArrays(1, &m_VertexArrayID);
	AddVertexBuffer(vertex_buffer);
	SetIndexBuffer(index_buffer);
}

VertexArray::VertexArray(std::initializer_list<VertexBuffer*> vertex_buffers, IndexBuffer* index_buffer)
{
	glCreateVertexArrays(1, &m_VertexArrayID);
	SetIndexBuffer(index_buffer);

	for(auto* buffer : vertex_buffers)
		AddVertexBuffer(buffer);
}

VertexArray::~VertexArray() { glDeleteVertexArrays(1, &m_VertexArrayID); }

void VertexArray::Bind() const
{
	glBindVertexArray(m_VertexArrayID);
}

void VertexArray::Unbind() const
{
	glBindVertexArray(0);
}

void VertexArray::AddVertexBuffer(VertexBuffer* vertex_buffer)
{
	if(!vertex_buffer)
	{
		VOLCANICORE_LOG_WARNING("Vertex buffer is nullptr");
		return;
	}

	glBindVertexArray(m_VertexArrayID);
	vertex_buffer->Bind();

	uint64_t offset = 0;
	const auto& layout = vertex_buffer->Layout;
	uint32_t stride = layout.Stride;

	if(layout.Elements.size() == 1) // Structure of Arrays
		stride = 0;

	for(auto& element : layout)
	{
		switch(element.Type)
		{
			case BufferDataType::Float:
			case BufferDataType::Vec2:
			case BufferDataType::Vec3:
			case BufferDataType::Vec4:
			{
				glEnableVertexAttribArray(m_BufferIndex);
				glVertexAttribPointer(
					m_BufferIndex, element.ComponentCount, GL_FLOAT, element.Normalized ? GL_FALSE : GL_TRUE, stride, (void*)offset
				);

				m_BufferIndex++;
				break;
			}

			case BufferDataType::Int:
			{
				glEnableVertexAttribArray(m_BufferIndex);
				glVertexAttribIPointer(m_BufferIndex, element.ComponentCount, GL_INT, stride, (void*)offset);

				m_BufferIndex++;
				break;
			}

			case BufferDataType::Mat2:
			case BufferDataType::Mat3:
			case BufferDataType::Mat4:
			{
				for(uint32_t i = 0; i < element.ComponentCount; i++)
				{
					glEnableVertexAttribArray(m_BufferIndex);
					glVertexAttribPointer(
						m_BufferIndex, element.ComponentCount, GL_FLOAT, element.Normalized ? GL_FALSE : GL_TRUE, stride,
						(void*)(offset + (sizeof(float) * element.ComponentCount * i))
					);

					glVertexAttribDivisor(m_BufferIndex, 1);
					m_BufferIndex++;
				}
				break;
			}
		}
		offset += element.Size;
	}

	m_VertexBuffers.push_back(vertex_buffer);
}

void VertexArray::SetIndexBuffer(IndexBuffer* index_buffer)
{
	if(!index_buffer)
	{
		VOLCANICORE_LOG_WARNING("Index buffer is nullptr");
		return;
	}

	glBindVertexArray(m_VertexArrayID);
	index_buffer->Bind();
	m_IndexBuffer = index_buffer;
}

}