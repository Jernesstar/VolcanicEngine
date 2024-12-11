#include "Mesh.h"

#include "Core/Log.h"

namespace VolcaniCore::OpenGL {

Mesh::Mesh(const std::string& path)
	: VolcaniCore::Mesh(path)
{
// 	BufferLayout layout({
// 		{ "Position", BufferDataType::Vec3 },
// 		{ "Normal",   BufferDataType::Vec3 },
// 		{ "TexCoord", BufferDataType::Vec4 }
// 	});

// 	Ref<VertexBuffer> vBuffer;
// 	Ref<IndexBuffer> indexBuffer;
// 	vBuffer = CreateRef<VertexBuffer>(layout, m_Vertices.size(), &m_Vertices[0]);
// 	indexBuffer = CreateRef<IndexBuffer>(m_Indices.size(), &m_Indices[0]);

// 	m_VertexArray = CreateRef<VertexArray>(vBuffer, indexBuffer);
}

Mesh::Mesh(const std::vector<Vertex>& verts,
			const std::vector<uint32_t>& indices, const Material& material)
	: VolcaniCore::Mesh(verts, indices, material)
{
	// BufferLayout layout({
	// 	{ "Position", BufferDataType::Vec3 },
	// 	{ "Normal",   BufferDataType::Vec3 },
	// 	{ "TexCoord", BufferDataType::Vec4 }
	// });

	// Ref<VertexBuffer> vBuffer;
	// Ref<IndexBuffer> indexBuffer;
	// vBuffer = CreateRef<VertexBuffer>(layout, m_Vertices.size(), &m_Vertices[0]);
	// indexBuffer = CreateRef<IndexBuffer>(m_Indices.size(), &m_Indices[0]);

	// m_VertexArray = CreateRef<VertexArray>(vBuffer, indexBuffer);
}

}