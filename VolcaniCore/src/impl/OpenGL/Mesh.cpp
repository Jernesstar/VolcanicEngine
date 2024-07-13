#include "Mesh.h"

namespace VolcaniCore::OpenGL {

Mesh::Mesh(std::vector<Vertex> vertices,
		   std::vector<uint32_t> indices,
		   Material material)
	: VolcaniCore::Mesh(vertices, indices, material)
{
	BufferLayout la({
		{ "Position", BufferDataType::Vec3 },
		{ "Normal",   BufferDataType::Vec3 },
		{ "TexCoord", BufferDataType::Vec2 }
	});

	m_VertexBuffer = CreateRef<VertexBuffer>(la, vertices.size(), &vertices[0]);
	m_IndexBuffer = CreateRef<IndexBuffer>(indices.size(), &indices[0]);
	
	m_VertexArray = CreateRef<VertexArray>(m_VertexBuffer, m_IndexBuffer);
}

}