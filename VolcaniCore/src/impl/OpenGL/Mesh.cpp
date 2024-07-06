#include "Mesh.h"

namespace VolcaniCore::OpenGL {

Mesh::Mesh(std::vector<Vertex> vertices,
		 std::vector<uint32_t> indices,
		 Material material)
	: VolcaniCore::Mesh(vertices, indices, material)
{
	BufferLayout layout({
		{ "Position", BufferDataType::Vec3 },
		{ "TexCoord", BufferDataType::Vec2 },
		{ "Normal",   BufferDataType::Vec3 }
	});

	m_VertexBuffer = CreateRef<VertexBuffer>(layout, m_Vertices.size(),
													&m_Vertices[0]);
	m_IndexBuffer = CreateRef<IndexBuffer>(indices.size(), &indices[0]);
	m_VertexArray = CreateRef<VertexArray>(m_VertexBuffer, m_IndexBuffer);
}

Mesh::~Mesh() { }

}