#include "Mesh.h"

#include "Core/Log.h"

namespace VolcaniCore::OpenGL {

Mesh::Mesh(const std::string& path)
	: VolcaniCore::Mesh(path)
{
	BufferLayout layout({
		{ "Position", BufferDataType::Vec3 },
		{ "Normal",   BufferDataType::Vec3 },
		{ "TexCoord_Color", BufferDataType::Vec4 }
	});

	m_VertexBuffer = 
		CreateRef<VertexBuffer>(layout, m_Vertices.size(), &m_Vertices[0]);
	m_IndexBuffer = CreateRef<IndexBuffer>(m_Indices.size(), &m_Indices[0]);

	m_VertexArray = CreateRef<VertexArray>(m_VertexBuffer, m_IndexBuffer);
	m_VertexArray->Unbind();
}

Mesh::Mesh(const std::vector<Vertex>& verts,
		   const std::vector<uint32_t>& indices,
		   const Material& material)
	: VolcaniCore::Mesh(verts, indices, material)
{
	BufferLayout layout({
		{ "Position", BufferDataType::Vec3 },
		{ "Normal",   BufferDataType::Vec3 },
		{ "TexCoord_Color", BufferDataType::Vec4 }
	});

	m_VertexBuffer = 
		CreateRef<VertexBuffer>(layout, m_Vertices.size(), &m_Vertices[0]);
	m_IndexBuffer = CreateRef<IndexBuffer>(m_Indices.size(), &m_Indices[0]);

	m_VertexArray = CreateRef<VertexArray>(m_VertexBuffer, m_IndexBuffer);
	m_VertexArray->Unbind();
}

}