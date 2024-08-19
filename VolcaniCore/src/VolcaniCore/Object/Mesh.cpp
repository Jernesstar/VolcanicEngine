#include "Mesh.h"

#include "Renderer/RendererAPI.h"

#include "Model.h"

#include "OpenGL/Mesh.h"

namespace VolcaniCore {

Mesh::Mesh(const std::string& path)
	: Path(path)
{
	Ref<Mesh> mesh = Model::Create(path)->GetMesh(0);
	this->m_Vertices = mesh->GetVertices();
	this->m_Indices  = mesh->GetIndices();
	this->m_Material = mesh->GetMaterial();
}

Ref<Mesh> Mesh::Create(const std::string& path) {
	RendererAPI::Backend backend = RendererAPI::GetBackend();

	switch(backend) {
		case RendererAPI::Backend::OpenGL:
			return CreateRef<OpenGL::Mesh>(path);
			break;
	}
}

Ref<Mesh> Mesh::Create(const std::vector<Vertex>& vertices,
					   const std::vector<uint32_t>& indices,
					   const Material& material)
{
	RendererAPI::Backend backend = RendererAPI::GetBackend();

	switch(backend) {
		case RendererAPI::Backend::OpenGL:
			return CreateRef<OpenGL::Mesh>(vertices, indices, material);
			break;
	}
}

Ref<Mesh> Mesh::Create(MeshPrimitive primitive, const Material& material) {
	std::vector<uint32_t> indices;
	std::vector<Vertex> vertices;

	switch(primitive) {
		case MeshPrimitive::Cube:
		{
			vertices =
			{
				{ { -0.5, -0.5, -0.5 }, {  0,  0, -1 }, { 0.0, 0.0, 0.0, 0.0 } },
				{ {  0.5, -0.5, -0.5 }, {  0,  0, -1 }, { 1.0, 0.0, 0.0, 0.0 } },
				{ {  0.5,  0.5, -0.5 }, {  0,  0, -1 }, { 1.0, 1.0, 0.0, 0.0 } },
				{ { -0.5,  0.5, -0.5 }, {  0,  0, -1 }, { 0.0, 1.0, 0.0, 0.0 } },
				{ { -0.5, -0.5,  0.5 }, {  0,  0,  1 }, { 0.0, 0.0, 0.0, 0.0 } },
				{ {  0.5, -0.5,  0.5 }, {  0,  0,  1 }, { 1.0, 0.0, 0.0, 0.0 } },
				{ {  0.5,  0.5,  0.5 }, {  0,  0,  1 }, { 1.0, 1.0, 0.0, 0.0 } },
				{ { -0.5,  0.5,  0.5 }, {  0,  0,  1 }, { 0.0, 1.0, 0.0, 0.0 } },

				{ { -0.5,  0.5, -0.5 }, { -1,  0,  0 }, { 0.0, 0.0, 0.0, 0.0 } },
				{ { -0.5, -0.5, -0.5 }, { -1,  0,  0 }, { 1.0, 0.0, 0.0, 0.0 } },
				{ { -0.5, -0.5,  0.5 }, { -1,  0,  0 }, { 1.0, 1.0, 0.0, 0.0 } },
				{ { -0.5,  0.5,  0.5 }, { -1,  0,  0 }, { 0.0, 1.0, 0.0, 0.0 } },
				{ {  0.5, -0.5, -0.5 }, {  1,  0,  0 }, { 0.0, 0.0, 0.0, 0.0 } },
				{ {  0.5,  0.5, -0.5 }, {  1,  0,  0 }, { 1.0, 0.0, 0.0, 0.0 } },
				{ {  0.5,  0.5,  0.5 }, {  1,  0,  0 }, { 1.0, 1.0, 0.0, 0.0 } },
				{ {  0.5, -0.5,  0.5 }, {  1,  0,  0 }, { 0.0, 1.0, 0.0, 0.0 } },

				{ { -0.5, -0.5, -0.5 }, {  0, -1,  0 }, { 0.0, 0.0, 0.0, 0.0 } },
				{ {  0.5, -0.5, -0.5 }, {  0, -1,  0 }, { 1.0, 0.0, 0.0, 0.0 } },
				{ {  0.5, -0.5,  0.5 }, {  0, -1,  0 }, { 1.0, 1.0, 0.0, 0.0 } },
				{ { -0.5, -0.5,  0.5 }, {  0, -1,  0 }, { 0.0, 1.0, 0.0, 0.0 } },
				{ {  0.5,  0.5, -0.5 }, {  0,  1,  0 }, { 0.0, 0.0, 0.0, 0.0 } },
				{ { -0.5,  0.5, -0.5 }, {  0,  1,  0 }, { 1.0, 0.0, 0.0, 0.0 } },
				{ { -0.5,  0.5,  0.5 }, {  0,  1,  0 }, { 1.0, 1.0, 0.0, 0.0 } },
				{ {  0.5,  0.5,  0.5 }, {  0,  1,  0 }, { 0.0, 1.0, 0.0, 0.0 } },
			};
			// index data
			indices =
			{
				0,  3,  2, // Front
				2,  1,  0,

				4,  5,  6, // Back
				6,  7,  4,

				11,  8,  9, // Left
				 9, 10, 11,

				12, 13, 14, // Right
				14, 15, 12,

				16, 17, 18, // Bottom
				18, 19, 16,

				20, 21, 22, // Top
				22, 23, 20
			};

			break;
		}

		case MeshPrimitive::Quad:
		{
			vertices =
			{
				{ { -0.5,  0.5,  0.5 }, { 0, 0, 1 }, { 0.0, 1.0, 0.0, 0.0 } },
				{ {  0.5,  0.5,  0.5 }, { 0, 0, 1 }, { 1.0, 1.0, 0.0, 0.0 } },
				{ { -0.5, -0.5,  0.5 }, { 0, 0, 1 }, { 0.0, 0.0, 0.0, 0.0 } },
				{ {  0.5, -0.5,  0.5 }, { 0, 0, 1 }, { 1.0, 0.0, 0.0, 0.0 } },
			};
			indices =
			{
				0, 2, 3,
				3, 1, 0,
			};

			break;
		}
		
		case MeshPrimitive::Line:
			break;
	}

	return Create(vertices, indices, material);
}

Ref<Mesh> Mesh::Create(MeshPrimitive primitive, const glm::vec4& color) {
	std::vector<uint32_t> indices;
	std::vector<Vertex> vertices;

	switch(primitive) {
		case MeshPrimitive::Cube:
		{
			vertices =
			{
				{ { -0.5, -0.5, -0.5 }, {  0,  0, -1 }, color },
				{ {  0.5, -0.5, -0.5 }, {  0,  0, -1 }, color },
				{ {  0.5,  0.5, -0.5 }, {  0,  0, -1 }, color },
				{ { -0.5,  0.5, -0.5 }, {  0,  0, -1 }, color },
				{ { -0.5, -0.5,  0.5 }, {  0,  0,  1 }, color },
				{ {  0.5, -0.5,  0.5 }, {  0,  0,  1 }, color },
				{ {  0.5,  0.5,  0.5 }, {  0,  0,  1 }, color },
				{ { -0.5,  0.5,  0.5 }, {  0,  0,  1 }, color },

				{ { -0.5,  0.5, -0.5 }, { -1,  0,  0 }, color },
				{ { -0.5, -0.5, -0.5 }, { -1,  0,  0 }, color },
				{ { -0.5, -0.5,  0.5 }, { -1,  0,  0 }, color },
				{ { -0.5,  0.5,  0.5 }, { -1,  0,  0 }, color },
				{ {  0.5, -0.5, -0.5 }, {  1,  0,  0 }, color },
				{ {  0.5,  0.5, -0.5 }, {  1,  0,  0 }, color },
				{ {  0.5,  0.5,  0.5 }, {  1,  0,  0 }, color },
				{ {  0.5, -0.5,  0.5 }, {  1,  0,  0 }, color },

				{ { -0.5, -0.5, -0.5 }, {  0, -1,  0 }, color },
				{ {  0.5, -0.5, -0.5 }, {  0, -1,  0 }, color },
				{ {  0.5, -0.5,  0.5 }, {  0, -1,  0 }, color },
				{ { -0.5, -0.5,  0.5 }, {  0, -1,  0 }, color },
				{ {  0.5,  0.5, -0.5 }, {  0,  1,  0 }, color },
				{ { -0.5,  0.5, -0.5 }, {  0,  1,  0 }, color },
				{ { -0.5,  0.5,  0.5 }, {  0,  1,  0 }, color },
				{ {  0.5,  0.5,  0.5 }, {  0,  1,  0 }, color },
			};
			// index data
			indices =
			{
				0,  3,  2, // Front
				2,  1,  0,

				4,  5,  6, // Back
				6,  7, 4,

				11,  8,  9, // Left
				 9, 10, 11,

				12, 13, 14, // Right
				14, 15, 12,

				16, 17, 18, // Bottom
				18, 19, 16,

				20, 21, 22, // Top
				22, 23, 20
			};

			break;
		}

		case MeshPrimitive::Quad:
		{
			vertices =
			{
				{ { -0.5,  0.5,  0.5 }, { 0, 0, 1 }, color },
				{ {  0.5,  0.5,  0.5 }, { 0, 0, 1 }, color },
				{ { -0.5, -0.5,  0.5 }, { 0, 0, 1 }, color },
				{ {  0.5, -0.5,  0.5 }, { 0, 0, 1 }, color },
			};
			indices =
			{
				0, 2, 3,
				3, 1, 0,
			};

			break;
		}
		
		case MeshPrimitive::Line:
			break;
	}

	return Create(vertices, indices);
}

}