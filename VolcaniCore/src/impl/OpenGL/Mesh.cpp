#include "Mesh.h"

namespace VolcaniCore::OpenGL {

Mesh::Mesh(std::vector<Vertex> vertices,
		   std::vector<uint32_t> indices,
		   Material material)
	: VolcaniCore::Mesh(vertices, indices, material)
{
	BufferLayout vertexLayout({
		{ "Position", BufferDataType::Vec3 },
		{ "Normal",   BufferDataType::Vec3 },
		{ "TexCoord", BufferDataType::Vec2 }
	});

	BufferLayout transformLayout = {
		{ "a_Transform", BufferDataType::Mat4 }
	};

	m_TransformBuffer = CreateRef<VertexBuffer>(transformLayout, 1000);
	m_VertexBuffer = CreateRef<VertexBuffer>(vertexLayout, vertices.size(),
											 &vertices[0]);
	m_IndexBuffer = CreateRef<IndexBuffer>(indices.size(), &indices[0]);
	
	m_VertexArray = CreateRef<VertexArray>();
	m_VertexArray->SetIndexBuffer(m_IndexBuffer);
	m_VertexArray->AddVertexBuffer(m_VertexBuffer);
	m_VertexArray->AddVertexBuffer(m_TransformBuffer);
}

Mesh::~Mesh() { }

}